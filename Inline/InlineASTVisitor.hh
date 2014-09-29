#ifndef VYGRAPH_INLINEASTVISITOR_HH
#define VYGRAPH_INLINEASTVISITOR_HH

#include <vector>

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "../Utility/ClangBaseWrapper.hh"

namespace vy {

class InlineASTVisitor : public clang::RecursiveASTVisitor<InlineASTVisitor> {
  clang::Rewriter& rewriter;
  clang::Rewriter::RewriteOptions opts;
  clang::ASTContext& context;

  void handleSimpleCallNoArgs(clang::CallExpr*) const;
  void handleNoArgs(clang::CallExpr*) const;
  void handleSimpleCallWithArgs(clang::CallExpr*) const;
  void handleArgs(clang::CallExpr*) const;

  void findRefInStmt(clang::Stmt*, std::vector<clang::DeclRefExpr*>&) const;
  void findSubstitutions(clang::Stmt*, std::vector<util::ClangBaseWrapper>&) const;

  std::string random_alphanum(size_t = 3) const;

public:
  InlineASTVisitor(clang::Rewriter&, clang::ASTContext&);
  bool VisitCallExpr(clang::CallExpr*);
};

} // namespace vy

#endif // VYGRAPH_INLINEASTVISITOR_HH
