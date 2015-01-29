#ifndef VYGRAPH_TRANSLATEACTION_HH
#define VYGRAPH_TRANSLATEACTION_HH

#include <sstream>

#include "clang/Frontend/FrontendAction.h"

namespace vy {

/**
  * @brief Class that represents the translate action
  *
  * @see http://clang.llvm.org/doxygen/classclang_1_1ASTFrontendAction.html
  * @see http://clang.llvm.org/docs/RAVFrontendAction.html
  */
class TranslateAction : public clang::ASTFrontendAction {
protected:
  clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance&,
                                        llvm::StringRef) override;

  bool BeginSourceFileAction(clang::CompilerInstance&, llvm::StringRef) override;

  void EndSourceFileAction() override;

private:
  std::stringstream outStream;
  std::string outFileName;

};

} // namespace vy

#endif // VYGRAPH_TRANSLATEACTION_HH

/** @file */
