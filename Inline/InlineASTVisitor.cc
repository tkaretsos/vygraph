#include "InlineASTVisitor.hh"

#include <iostream>
#include "../Analysis/FunctionManager.hh"

namespace vy {

using namespace clang;
using namespace std;

InlineASTVisitor::InlineASTVisitor(Rewriter& rewriter, ASTContext& context)
  : rewriter(rewriter), context(context) {
  opts.RemoveLineIfEmpty = true;
}

bool InlineASTVisitor::VisitCallExpr(CallExpr* call) {
  FunctionDecl* funcDecl = call->getDirectCallee();
  if (functionMgr.isUserDefined(funcDecl->getNameAsString())) {
    CompoundStmt* funcBody = cast<CompoundStmt>(funcDecl->getBody());

    // eg: void foo() { }
    // *code* ...
    // foo();
    // *code* ...
    if (call->getNumArgs() == 0 && functionMgr.isSimpleCall(call)) {

      // delete the call text
      rewriter.RemoveText(call->getSourceRange());
      rewriter.RemoveText(call->getLocStart(), 1); // the semicolon

      for (auto i = funcBody->body_begin(); i != funcBody->body_end(); i++) {
        if (!isa<ReturnStmt>(*i)) {
          rewriter.InsertText(call->getLocStart(),
                              rewriter.ConvertToString(*i), true, true);
          if (isa<Expr>(*i)) {
            rewriter.InsertText(call->getLocStart(), ";\n", true, true);
          }
        }
      }
    }

    // eg: <nonvoid> foo() { ... return <expr>; }
    // *code* ...
    // x = <expr> <operator> foo() <operator> <expr>;
    // *code* ...
    if (call->getNumArgs() == 0 && !functionMgr.isSimpleCall(call)) {
      string varName(call->getDirectCallee()->getNameAsString() + "_retval");
      auto startLoc = functionMgr.getStmtLoc(call);
      auto retType = call->getCallReturnType();

      for (auto i = funcBody->body_begin(); i != funcBody->body_end(); i++) {
        if (!isa<ReturnStmt>(*i)) {
          rewriter.InsertText(startLoc,
                              rewriter.ConvertToString(*i), true, true);
          if (isa<Expr>(*i)) {
            rewriter.InsertText(startLoc, ";\n", true, true);
          }
        } else {
          auto retStmt = cast<ReturnStmt>(*i);
          auto retVal = rewriter.ConvertToString(*retStmt->children().first);
          string newStr("");
          newStr += retType.getAsString() + " ";
          newStr += varName + " = ";
          newStr += retVal + ";\n";
          rewriter.InsertText(startLoc, newStr, true, true);
        }
      }
      rewriter.ReplaceText(call->getSourceRange(), varName);
    }
  }

  return true;
}

} // namespace vy
