#ifndef VYGRAPH_INLINEACTION_HH
#define VYGRAPH_INLINEACTION_HH

#include "clang/Frontend/FrontendAction.h"
#include "clang/Rewrite/Core/Rewriter.h"

namespace vy {

class InlineAction : public clang::ASTFrontendAction {
  clang::Rewriter rewriter;
public:
  InlineAction();
  clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance&,
                                        llvm::StringRef) override;
  void EndSourceFileAction() override;
};

} // namespace vy

#endif // VYGRAPH_INLINEACTION_HH
