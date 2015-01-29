#ifndef VYGRAPH_INLINEACTION_HH
#define VYGRAPH_INLINEACTION_HH

#include "clang/Frontend/FrontendAction.h"
#include "clang/Rewrite/Core/Rewriter.h"

namespace vy {

/**
 * @brief Class that represents the inline action
 *
 * @see http://clang.llvm.org/doxygen/classclang_1_1ASTFrontendAction.html
 * @see http://clang.llvm.org/docs/RAVFrontendAction.html
 */
class InlineAction : public clang::ASTFrontendAction {
public:
  clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance&,
                                        llvm::StringRef) override;
  void EndSourceFileAction() override;

private:
  clang::Rewriter rewriter;

  /**
   * @brief Write the result of the action to a file.
   */
  void writeToFile();

  /**
   * @brief Write the result of the action to a string so that later actions
   * can use it.
   */
  void writeToMemory();

};

} // namespace vy

#endif // VYGRAPH_INLINEACTION_HH

/** @file */
