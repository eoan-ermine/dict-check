#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

class DeclPrinter : public MatchFinder::MatchCallback {
public:
  virtual void run(const MatchFinder::MatchResult &Result) override {
    if (const NamedDecl *Decl = Result.Nodes.getNodeAs<clang::NamedDecl>("decl")) {
      llvm::outs() << "Found declaration of " << Decl->getNameAsString() << '\n';
    }
  }
};

static llvm::cl::OptionCategory DictCheckCategory("dict-check options");
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, DictCheckCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser& OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  DeclPrinter Printer;

  MatchFinder Finder;
  Finder.addMatcher(declaratorDecl().bind("decl"), &Printer);
  Finder.addMatcher(cxxRecordDecl().bind("decl"), &Printer);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}
