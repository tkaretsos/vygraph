#ifndef VYGRAPH_TRANSLATEACTION_HH
#define VYGRAPH_TRANSLATEACTION_HH

#include <sstream>

#include "clang/Frontend/FrontendAction.h"

namespace vy {

class TranslateAction : public clang::ASTFrontendAction {
public:
  TranslateAction();

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
