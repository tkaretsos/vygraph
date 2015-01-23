#ifndef VYGRAPH_INLINEASTCONSUMER_HH
#define VYGRAPH_INLINEASTCONSUMER_HH

#include "clang/AST/ASTConsumer.h"

#include "InlineASTVisitor.hh"

namespace vy {

class InlineASTConsumer : public clang::ASTConsumer {
public:
  InlineASTConsumer(clang::Rewriter&, clang::ASTContext&);
  bool HandleTopLevelDecl(clang::DeclGroupRef) override;

private:
  InlineASTVisitor inlineVisitor;

};

} // namespace vy

#endif // VYGRAPH_INLINEASTCONSUMER_HH
