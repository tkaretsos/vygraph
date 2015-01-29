#ifndef VYGRAPH_TRANSLATEASTCONSUMER_HH
#define VYGRAPH_TRANSLATEASTCONSUMER_HH

#include "clang/AST/ASTConsumer.h"

#include "Translate/Translator.hh"

namespace vy {

/**
  * @see http://clang.llvm.org/doxygen/classclang_1_1ASTConsumer.html
  * @see http://clang.llvm.org/docs/RAVFrontendAction.html
  */
class TranslateASTConsumer : public clang::ASTConsumer {
public:
  TranslateASTConsumer(clang::ASTContext&, std::stringstream&);
  bool HandleTopLevelDecl(clang::DeclGroupRef) override;

private:
  Translator translator;

};

} // namespace vy

#endif // VYGRAPH_TRANSLATEASTCONSUMER_HH

/** @file */
