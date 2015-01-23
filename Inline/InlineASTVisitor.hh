#ifndef VYGRAPH_INLINEASTVISITOR_HH
#define VYGRAPH_INLINEASTVISITOR_HH

#include "clang/AST/RecursiveASTVisitor.h"

#include "Inline/Inliner.hh"

namespace clang {

class ASTContext;
class Rewriter;

}

namespace vy {

class InlineASTVisitor : public clang::RecursiveASTVisitor<InlineASTVisitor> {
public:
  InlineASTVisitor(clang::Rewriter&, clang::ASTContext&);
  bool VisitCallExpr(clang::CallExpr*);

private:
  Inliner inliner;

};

} // namespace vy

#endif // VYGRAPH_INLINEASTVISITOR_HH
