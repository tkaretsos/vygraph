#include "InlineASTVisitor.hh"

#include "Analysis/FunctionManager.hh"

namespace vy {

using namespace clang;
using namespace std;

InlineASTVisitor::InlineASTVisitor(Rewriter& rewriter, ASTContext& context)
  : inliner(context, rewriter)
{ }

bool
InlineASTVisitor::VisitCallExpr(CallExpr* call) {
  if (functionMgr.isUserDefined(call->getDirectCallee()->getNameAsString())) {
    inliner.Init(call);
    inliner.Inline();
  }

  return true;
}

} // namespace vy

/** @file */
