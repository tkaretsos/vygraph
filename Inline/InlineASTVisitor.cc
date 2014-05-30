#include "InlineASTVisitor.hh"

namespace vy {

using namespace clang;

InlineASTVisitor::InlineASTVisitor(Rewriter& rewriter, ASTContext& context)
  : rewriter(rewriter), context(context) {
  opts.RemoveLineIfEmpty = true;
}

bool InlineASTVisitor::VisitCallExpr(CallExpr* call) {
  FunctionDecl* f = call->getDirectCallee();
  // we need to distinguish from functions defined in other files, like printf
  if (f->hasBody()) {
    CompoundStmt* body = static_cast<CompoundStmt*>(f->getBody());

    // delete the call text
    rewriter.RemoveText(call->getSourceRange());
    rewriter.RemoveText(call->getLocStart(), 1); // the semicolon

    for (auto i = body->body_begin(); i != body->body_end(); i++) {
      if (isa<ReturnStmt>(*i))
        continue;
      rewriter.InsertText(call->getLocStart(),
                          rewriter.ConvertToString(*i), true, true);
      if (isa<Expr>(*i))
        rewriter.InsertText(call->getLocStart(), ";\n", true, true);
    }

//    rewriter.RemoveText(f->getSourceRange(), opts);
  }

  return true;
}

} // namespace vy
