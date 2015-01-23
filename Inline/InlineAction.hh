#ifndef VYGRAPH_INLINEACTION_HH
#define VYGRAPH_INLINEACTION_HH

#include "clang/Frontend/FrontendAction.h"
#include "clang/Rewrite/Core/Rewriter.h"

namespace vy {

class InlineAction : public clang::ASTFrontendAction {
public:
  clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance&,
                                        llvm::StringRef) override;
  void EndSourceFileAction() override;

private:
  clang::Rewriter rewriter;

  void writeToFile();
  void writeToMemory();

};

} // namespace vy

#endif // VYGRAPH_INLINEACTION_HH
