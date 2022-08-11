#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "../../common/io.hpp"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

class DeclPrinter : public MatchFinder::MatchCallback {
public:
  DeclPrinter(const std::unordered_set<std::string>& forbiddenIdents)
    : forbiddenIdents(forbiddenIdents) { }
  virtual void run(const MatchFinder::MatchResult &Result) override {
    if (const NamedDecl *Decl = Result.Nodes.getNodeAs<clang::NamedDecl>("decl")) {
      std::string name = Decl->getNameAsString();
      if (forbiddenIdents.find(name) != forbiddenIdents.end()) {
        FullSourceLoc FullLocation = Result.Context->getFullLoc(Decl->getBeginLoc());
        if (FullLocation.isValid()) {
          llvm::errs() << "found declaration with forbidden name " << name
                       << " at " << FullLocation.getSpellingLineNumber() << ":"
                       << FullLocation.getSpellingColumnNumber() << "\n";
        }
      }
    }
  }
private:
  const std::unordered_set<std::string>& forbiddenIdents;
};

static llvm::cl::OptionCategory DictCheckCategory("dict-check options");
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::opt<std::string> DictLocation(cl::Positional, cl::Required, cl::desc("<dictionary file>"));

int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, DictCheckCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser& OptionsParser = ExpectedParser.get();

  std::unordered_set<std::string> forbiddenIdents;  
  if (readDictionaryFile(DictLocation, forbiddenIdents) == 0) {
    llvm::errs() << "failed to open " << DictLocation << '\n';
    return 1;
  }

  DeclPrinter Printer(forbiddenIdents);
  MatchFinder Finder;
  Finder.addMatcher(declaratorDecl().bind("decl"), &Printer);
  Finder.addMatcher(cxxRecordDecl().bind("decl"), &Printer);

  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  return Tool.run(newFrontendActionFactory(&Finder).get());
}
