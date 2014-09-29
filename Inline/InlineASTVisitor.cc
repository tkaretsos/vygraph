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
void InlineASTVisitor::handleSimpleCallNoArgs(CallExpr* call) const {
  // delete the call text
  rewriter.RemoveText(call->getSourceRange());
  rewriter.RemoveText(call->getLocStart(), 1); // the semicolon and the new line

  auto body = cast<CompoundStmt>(call->getDirectCallee()->getBody());
  for (auto s = body->body_begin(); s != body->body_end(); ++s) {
    if (isa<ReturnStmt>(*s))
      continue;
    string insertStr = rewriter.ConvertToString((*s));
    if (isa<Expr>(*s))
      insertStr.append(";\n");
    rewriter.InsertText(call->getLocStart(), insertStr, true, true);
  }
}

// eg: <nonvoid> foo() { ... return <expr>; }
// *code* ...
// x = <expr> <operator> foo() <operator> <expr>;
// *code* ...
void InlineASTVisitor::handleNoArgs(CallExpr* call) const {
  string callReplacement("(");

  auto body = cast<CompoundStmt>(call->getDirectCallee()->getBody());
  for (auto s = body->body_begin(); s != body->body_end(); ++s) {
    string insertStr = rewriter.ConvertToString(*s);
    if (isa<ReturnStmt>(*s)) {
      callReplacement.append(insertStr.begin() + 7, insertStr.end() - 2);
      callReplacement.push_back(')');
    } else {
      if (isa<Expr>(*s))
        insertStr.append(";\n");
      rewriter.InsertText(functionMgr.getStmtLoc(call), insertStr, true, true);
    }
  }
  rewriter.ReplaceText(call->getSourceRange(), callReplacement);
}

// eg: void foo(<type> param1, ...) { ... return <expr>; }
// *code* ...
// foo(arg1, ...);
// *code* ...
void InlineASTVisitor::handleSimpleCallWithArgs(CallExpr* call) const {
  // delete the call text
  rewriter.RemoveText(call->getSourceRange());
  rewriter.RemoveText(call->getLocStart(), 1);

  // create a map between the parameters of the function
  // and the arguments of the call
  map<string, Expr*> varMap;
  auto param = call->getDirectCallee()->param_begin();
  for (auto arg = call->arg_begin(); arg != call->arg_end(); ++arg, ++param) {
    varMap[(*param)->getNameAsString()] = *arg;
  }

  auto body = cast<CompoundStmt>(call->getDirectCallee()->getBody());
  for (auto s = body->body_begin(); s != body->body_end(); ++s) {
    if (isa<ReturnStmt>(*s))
      continue;

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

    if (isa<Expr>(*s))
      insertStr.append(";\n");
    rewriter.InsertText(call->getLocStart(), insertStr, true, true);
  }
}

// eg: <nonvoid> foo(<type> param1, ...) { ... return <expr>; }
// *code* ...
// x = <expr> <operator> foo(arg1, ...) <operator> <expr>;
// *code* ...
void InlineASTVisitor::handleArgs(CallExpr* call) const {
  string callReplacement("(");

  // create a map between the parameters of the function
  // and the arguments of the call
  map<string, Expr*> varMap;
  auto param = call->getDirectCallee()->param_begin();
  for (auto arg = call->arg_begin(); arg != call->arg_end(); ++arg, ++param) {
    varMap[(*param)->getNameAsString()] = *arg;
  }

  auto body = cast<CompoundStmt>(call->getDirectCallee()->getBody());
  for (auto s = body->body_begin(); s != body->body_end(); ++s) {
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

    if (isa<ReturnStmt>(*s)) {
      callReplacement.append(insertStr.begin() + 7, insertStr.end() - 2);
      callReplacement.push_back(')');
    } else {
      if (isa<Expr>(*s))
        insertStr.append(";\n");
      rewriter.InsertText(functionMgr.getStmtLoc(call), insertStr, true, true);
    }
  }
  rewriter.ReplaceText(call->getSourceRange(), callReplacement);
}

void InlineASTVisitor::findRefInStmt(Stmt* stmt, vector<DeclRefExpr*>& declRefs) const {
  if (DeclRefExpr* ref = dyn_cast<DeclRefExpr>(stmt))
    declRefs.push_back(ref);
  for (auto c : stmt->children())
    findRefInStmt(c, declRefs);
}

string InlineASTVisitor::random_alphanum(size_t length) const {
  const char charset[] = "0123456789"
                         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "abcdefghijklmnopqrstuvwxyz";
  string str(length, 0);
  auto randchar = [&]() { return charset[rand() % (sizeof(charset) - 1)]; };
  generate_n(str.begin(), length, randchar);
  return str;
}



} // namespace vy
