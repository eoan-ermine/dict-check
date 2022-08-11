#include <unordered_set>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;

class FindDeclVisitor
  : public RecursiveASTVisitor<FindDeclVisitor> {
public:
  FindDeclVisitor(ASTContext *context, const std::unordered_set<std::string>& forbiddenIdents)
    : context(context), forbiddenIdents(forbiddenIdents) { }

  bool VisitDeclaratorDecl(DeclaratorDecl *Declaration) {
    handleDecl(Declaration);
    return true;
  }
  bool VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
    handleDecl(Declaration);
    return true;
  }
private:
  ASTContext *context;
  const std::unordered_set<std::string>& forbiddenIdents;
  void handleDecl(NamedDecl *Decl) {
    std::string name = Decl->getNameAsString();
    if (forbiddenIdents.find(name) != forbiddenIdents.end()) {
      FullSourceLoc FullLocation = context->getFullLoc(Decl->getBeginLoc());
      if (FullLocation.isValid()) {
        llvm::outs() << "found forbidden name " << name
                     << " at " << FullLocation.getSpellingLineNumber() << ":"
                     << FullLocation.getSpellingColumnNumber() << "\n";
      }
    }
  }

};

class FindDeclConsumer : public clang::ASTConsumer {
public:
  FindDeclConsumer(ASTContext* context, const std::unordered_set<std::string>& forbiddenIdents)
    : Visitor(context, forbiddenIdents) { }
  virtual void HandleTranslationUnit(clang::ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  FindDeclVisitor Visitor;
};

class FindDeclAction : public clang::ASTFrontendAction {
public:
  FindDeclAction(const std::unordered_set<std::string>& forbiddenIdents)
    : forbiddenIdents(forbiddenIdents) { }
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) override {
    return std::make_unique<FindDeclConsumer>(&Compiler.getASTContext(), forbiddenIdents);
  }
private:
  const std::unordered_set<std::string>& forbiddenIdents;
};

int readSourceFile(const std::string& filename, std::string& sourceCode) {
  std::ostringstream sourceBuffer;
  std::ifstream stream(filename);
  std::string line;

  if(!stream.is_open()) {
    return 0;
  }

  while (std::getline(stream, line)) {
    sourceBuffer << line << '\n';
  }

  sourceCode = sourceBuffer.str();
  return 1;
}

int readDictionaryFile(const std::string& filename, std::unordered_set<std::string>& dictionary) {
  std::ifstream stream(filename);
  std::string word;

  if (!stream.is_open()) {
    return 0;
  }

  while (stream >> word) {
    dictionary.insert(word);
  }

  return 1;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    llvm::outs() << "Format: dict-check [path-to-dictionary] [path-to-source-code]" << '\n';
    return 1;
  }

  std::unordered_set<std::string> dictionary;  
  if (readDictionaryFile(argv[1], dictionary) == 0) {
    llvm::outs() << "failed to open " << argv[1] << '\n';
    return 1;
  }

  std::string sourceCode;  
  if (readSourceFile(argv[2], sourceCode) == 0) {
    llvm::outs() << "failed to open " << argv[2] << '\n';
    return 1;
  }

  clang::tooling::runToolOnCode(std::make_unique<FindDeclAction>(dictionary), sourceCode);
}
