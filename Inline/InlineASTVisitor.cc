#include "InlineASTVisitor.hh"

#include <iostream>
#include <algorithm>
#include <map>
#include "../Analysis/FunctionManager.hh"

namespace vy {

using namespace clang;
using namespace std;

InlineASTVisitor::InlineASTVisitor(Rewriter& rewriter, ASTContext& context)
  : rewriter(rewriter), context(context) {
  opts.RemoveLineIfEmpty = true;
}

bool InlineASTVisitor::VisitCallExpr(CallExpr* call) {
  if (functionMgr.isUserDefined(call->getDirectCallee()->getNameAsString())) {
    if (call->getNumArgs() == 0) {
      if (functionMgr.isSimpleCall(call)) {
        handleSimpleCallNoArgs(call);
      } else {
        handleNoArgs(call);
      }
    } else {
      if (functionMgr.isSimpleCall(call)) {
        handleSimpleCallWithArgs(call);
      } else {
        handleArgs(call);
      }
    }
  }

  return true;
}

// eg: void foo() { }
// *code* ...
// foo();
// *code* ...
void InlineASTVisitor::handleSimpleCallNoArgs(clang::CallExpr* call) const {
  CompoundStmt* funcBody = cast<CompoundStmt>(call->getDirectCallee()->getBody());

  // delete the call text
  rewriter.RemoveText(call->getSourceRange());
  rewriter.RemoveText(call->getLocStart(), 1); // the semicolon and the new line

  for (auto s = funcBody->body_begin(); s != funcBody->body_end(); s++) {
    if (!isa<ReturnStmt>(*s)) {
      rewriter.InsertText(call->getLocStart(),
                          rewriter.ConvertToString(*s), true, true);
      if (isa<Expr>(*s)) {
        rewriter.InsertText(call->getLocStart(), ";\n", true, true);
      }
    }
  }
}

// eg: <nonvoid> foo() { ... return <expr>; }
// *code* ...
// x = <expr> <operator> foo() <operator> <expr>;
// *code* ...
void InlineASTVisitor::handleNoArgs(CallExpr* call) const {
  CompoundStmt* funcBody = cast<CompoundStmt>(call->getDirectCallee()->getBody());
  string varName(call->getDirectCallee()->getNameAsString() + "_retval");
  auto startLoc = functionMgr.getStmtLoc(call);
  auto retType = call->getCallReturnType();

  for (auto s = funcBody->body_begin(); s != funcBody->body_end(); s++) {
    if (!isa<ReturnStmt>(*s)) {
      rewriter.InsertText(startLoc,
                          rewriter.ConvertToString(*s), true, true);
      if (isa<Expr>(*s)) {
        rewriter.InsertText(startLoc, ";\n", true, true);
      }
    } else {
      auto retStmt = cast<ReturnStmt>(*s);
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

// eg: void foo(<type> param1, ...) { ... return <expr>; }
// *code* ...
// foo(arg1, ...);
// *code* ...
void InlineASTVisitor::handleSimpleCallWithArgs(CallExpr* call) const {

  rewriter.RemoveText(call->getSourceRange());
  rewriter.RemoveText(call->getLocStart(), 1); // the semicolon and the new line

  // create a map between the parameters of the function
  // and the arguments of the call
  map<string, Expr*> varMap;
  auto param = call->getDirectCallee()->param_begin();
  for (auto arg = call->arg_begin(); arg != call->arg_end(); ++arg, ++param) {
    varMap[(*param)->getNameAsString()] = *arg;
  }

  auto body = cast<CompoundStmt>(call->getDirectCallee()->getBody());
  for (auto s = body->body_begin(); s != body->body_end(); ++s) {
    if (isa<ReturnStmt>(*s)) {
      continue;
    }
    string insertStr = rewriter.ConvertToString((*s));
    vector<DeclRefExpr*> declRefs;
    findRefInStmt(*s, declRefs);
    reverse(declRefs.begin(), declRefs.end());
    for (auto declRef : declRefs) {
      auto found = varMap.find(declRef->getFoundDecl()->getNameAsString());
      if (found != varMap.end()) {
        auto offset = declRef->getLocStart().getRawEncoding() -
                      (*s)->getLocStart().getRawEncoding();
        auto begin = insertStr.begin() + offset;
        string exprStr("(" + rewriter.ConvertToString(found->second) + ")");
        insertStr.replace(begin, begin + found->first.length(), exprStr);
      }
    }

    if (isa<Expr>(*s)) {
      insertStr.append(";\n");
    }
    rewriter.InsertText(call->getLocStart(), insertStr, true, true);
  }
}

// eg: <nonvoid> foo(<type> param1, ...) { ... return <expr>; }
// *code* ...
// x = <expr> <operator> foo(arg1, ...) <operator> <expr>;
// *code* ...
void InlineASTVisitor::handleArgs(CallExpr* call) const {
  CompoundStmt* funcBody = cast<CompoundStmt>(call->getDirectCallee()->getBody());
  string varName(call->getDirectCallee()->getNameAsString() + "_retval");
  auto startLoc = functionMgr.getStmtLoc(call);
  auto retType = call->getCallReturnType();
  map<string, Expr*> varMap;
  map<Stmt*, string> rollbacks;

  auto param = call->getDirectCallee()->param_begin();
  for (auto arg = call->arg_begin(); arg != call->arg_end(); ++arg, ++param) {
    varMap[(*param)->getNameAsString()] = *arg;
  }

//   for (auto v : varMap) {
//     std::cout << v.first << " to " << rewriter.ConvertToString(v.second) << std::endl;
//   }
  for (auto s = funcBody->body_begin(); s != funcBody->body_end(); ++s) {
    rollbacks[*s] = rewriter.ConvertToString(*s);
    if (isa<ReturnStmt>(*s)) {
      rollbacks[*s].erase(rollbacks[*s].end() - 2, rollbacks[*s].end());
    } else {
      rollbacks[*s].pop_back();
    }

    vector<DeclRefExpr*> declRefs;
    findRefInStmt(*s, declRefs);
    for (auto declRef : declRefs) {
      auto found = varMap.find(declRef->getFoundDecl()->getNameAsString());
      if (found != varMap.end()) {
        rewriter.ReplaceText(declRef->getSourceRange(),
                             "(" + rewriter.ConvertToString(found->second) + ")");
      }
    }

    if (!isa<ReturnStmt>(*s)) {
      string newText(rewriter.getRewrittenText((*s)->getSourceRange()));
      if (isa<Expr>(*s)) {
        newText.push_back(';');
      }
      newText.push_back('\n');
      rewriter.InsertText(startLoc, newText, true, true);
    } else {
      auto retStmt = cast<ReturnStmt>(*s);
      auto retVal = rewriter.ConvertToString(*retStmt->children().first);
      string newStr("");
      newStr += retType.getAsString() + " ";
      newStr += varName + " = ";
      newStr += retVal + ";\n";
      rewriter.InsertText(startLoc, newStr, true, true);
    }
  }
  rewriter.ReplaceText(call->getSourceRange(), varName);

  for (auto r : rollbacks) {
    rewriter.ReplaceText(r.first->getSourceRange(), r.second);
  }
}


void InlineASTVisitor::findRefInStmt(Stmt* stmt, vector<DeclRefExpr*>& declRefs) const {

  if (DeclRefExpr* ref = dyn_cast<DeclRefExpr>(stmt)) {
    declRefs.push_back(ref);
  }

  for (auto c : stmt->children()) {
    findRefInStmt(c, declRefs);
  }
}


} // namespace vy
