#ifndef VYGRAPH_INLINEASTCONSUMER_HH
#define VYGRAPH_INLINEASTCONSUMER_HH

#include "clang/AST/ASTConsumer.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include "InlineASTVisitor.hh"

namespace vy {

class InlineASTConsumer : public clang::ASTConsumer {
  InlineASTVisitor inliner;
public:
  InlineASTConsumer(clang::Rewriter&, clang::ASTContext&);
  bool HandleTopLevelDecl(clang::DeclGroupRef) override;
};

} // namespace vy

#endif // VYGRAPH_INLINEASTCONSUMER_HH
