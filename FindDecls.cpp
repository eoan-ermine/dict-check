#include <iostream>
#include <sstream>
#include <fstream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;

class FindDeclVisitor
  : public RecursiveASTVisitor<FindDeclVisitor> {
public:
  bool VisitDeclaratorDecl(DeclaratorDecl *Declaration) {
    print(Declaration);
    return true;
  }
  bool VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
    print(Declaration);
    return true;
  }
private:
  void print(NamedDecl *Decl) {
    llvm::outs() << "Find declaration of " << Decl->getNameAsString() << '\n';
  }
};

class FindDeclConsumer : public clang::ASTConsumer {
public:
  virtual void HandleTranslationUnit(clang::ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  FindDeclVisitor Visitor;
};

class FindDeclAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) override {
    return std::make_unique<FindDeclConsumer>();
  }
};

int main(int argc, char **argv) {
  if (argc > 1) {
    std::ostringstream sourceBuffer;
    std::ifstream stream(argv[1]);
    std::string line; 

    if(!stream.is_open()) {
      llvm::outs() << "failed to open " << argv[1] << '\n';
      return 1;
    }

    while (std::getline(stream, line)) {
      sourceBuffer << line << '\n';
    }

    clang::tooling::runToolOnCode(std::make_unique<FindDeclAction>(), sourceBuffer.str());
  }
}
