#include "InlineASTVisitor.hh"

#include <iostream>
#include <algorithm>
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

    auto subMap(functionMgr.getVarSubs(call));
    for (auto& i : subMap)
      i.second = i.first + "_" + random_alphanum();

    if (call->getNumArgs() == 0) {
      if (functionMgr.isSimpleCall(call))
        handleSimpleCallNoArgs(call, subMap);
      else
        handleNoArgs(call, subMap);
    } else {
      if (functionMgr.isSimpleCall(call))
        handleSimpleCallWithArgs(call, subMap);
      else
        handleArgs(call, subMap);
    }
  }

  return true;
}

// eg: void foo() { }
// *code* ...
// foo();
// *code* ...
void InlineASTVisitor::handleSimpleCallNoArgs(CallExpr* call,
                                              const map<string, string>& subMap) const {
  // delete the call text
  rewriter.RemoveText(call->getSourceRange());
  rewriter.RemoveText(call->getLocStart(), 1);

  auto body = cast<CompoundStmt>(call->getDirectCallee()->getBody());
  for (auto s = body->body_begin(); s != body->body_end(); ++s) {
    if (isa<ReturnStmt>(*s))
      continue;
    string insertStr = rewriter.ConvertToString((*s));

    vector<util::ClangBaseWrapper> subs;
    findSubstitutions(*s, subs);
    reverse(subs.begin(), subs.end());
    for (auto& sub : subs) {
      auto found = subMap.find(sub.getAsString(rewriter));
      if (found != subMap.end()) {
        auto offset = sub.getLocStart().getRawEncoding() -
                      (*s)->getLocStart().getRawEncoding();
        auto begin = insertStr.begin() + offset;
        insertStr.replace(begin, begin + found->first.length(), found->second);
      }
    }

    if (isa<Expr>(*s))
      insertStr.append(";\n");
    rewriter.InsertText(call->getLocStart(), insertStr, true, true);
  }
}

// eg: <nonvoid> foo() { ... return <expr>; }
// *code* ...
// x = <expr> <operator> foo() <operator> <expr>;
// *code* ...
void InlineASTVisitor::handleNoArgs(CallExpr* call,
                                    const map<string, string>& subMap) const {
  string callReplacement;

  auto body = cast<CompoundStmt>(call->getDirectCallee()->getBody());
  for (auto s = body->body_begin(); s != body->body_end(); ++s) {
    string insertStr = rewriter.ConvertToString(*s);

    vector<util::ClangBaseWrapper> subs;
    findSubstitutions(*s, subs);
    reverse(subs.begin(), subs.end());
    for (auto& sub : subs) {
      auto found = subMap.find(sub.getAsString(rewriter));
      if (found != subMap.end()) {
        auto offset = sub.getLocStart().getRawEncoding() -
                      (*s)->getLocStart().getRawEncoding();
        auto begin = insertStr.begin() + offset;
        insertStr.replace(begin, begin + found->first.length(), found->second);
      }
    }

    if (isa<ReturnStmt>(*s)) {
      callReplacement.push_back('(');
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
void InlineASTVisitor::handleSimpleCallWithArgs(CallExpr* call,
                                                const map<string, string>& subMap) const {
  // delete the call text
  rewriter.RemoveText(call->getSourceRange());
  rewriter.RemoveText(call->getLocStart(), 1);

  auto param = call->getDirectCallee()->param_begin();
  for (auto arg = call->arg_begin(); arg != call->arg_end(); ++arg, ++param) {
    string insertStr((*param)->getOriginalType().getAsString() + " ");
    insertStr.append(subMap.at((*param)->getNameAsString()) + " = ");
    insertStr.append(rewriter.ConvertToString(*arg) + ";\n");
    rewriter.InsertText(call->getLocStart(), insertStr, true, true);
  }

  auto body = cast<CompoundStmt>(call->getDirectCallee()->getBody());
  for (auto s = body->body_begin(); s != body->body_end(); ++s) {
    if (isa<ReturnStmt>(*s))
      continue;

    string insertStr = rewriter.ConvertToString((*s));
    vector<util::ClangBaseWrapper> subs;
    findSubstitutions(*s, subs);
    reverse(subs.begin(), subs.end());
    for (auto& sub : subs) {
      auto found = subMap.find(sub.getAsString(rewriter));
      if (found != subMap.end()) {
        auto offset = sub.getLocStart().getRawEncoding() -
                      (*s)->getLocStart().getRawEncoding();
        auto begin = insertStr.begin() + offset;
        insertStr.replace(begin, begin + found->first.length(), found->second);
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
void InlineASTVisitor::handleArgs(CallExpr* call,
                                  const map<string, string>& subMap) const {
  string callReplacement;

  auto param = call->getDirectCallee()->param_begin();
  for (auto arg = call->arg_begin(); arg != call->arg_end(); ++arg, ++param) {
    string varName(subMap.at((*param)->getNameAsString()));
    string insertStr((*param)->getOriginalType().getAsString() + " ");
    insertStr.append(varName + " = ");
    insertStr.append(rewriter.ConvertToString(*arg) + ";\n");
    rewriter.InsertText(functionMgr.getStmtLoc(call), insertStr, true, true);
  }

  auto body = cast<CompoundStmt>(call->getDirectCallee()->getBody());
  for (auto s = body->body_begin(); s != body->body_end(); ++s) {
    string insertStr = rewriter.ConvertToString((*s));

    vector<util::ClangBaseWrapper> subs;
    findSubstitutions(*s, subs);
    reverse(subs.begin(), subs.end());
    for (auto& sub : subs) {
      auto found = subMap.find(sub.getAsString(rewriter));
      if (found != subMap.end()) {
        auto offset = sub.getLocStart().getRawEncoding() -
                      (*s)->getLocStart().getRawEncoding();
        auto begin = insertStr.begin() + offset;
        insertStr.replace(begin, begin + found->first.length(), found->second);
      }
    }

    if (isa<ReturnStmt>(*s)) {
      callReplacement.push_back('(');
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

void InlineASTVisitor::findSubstitutions(Stmt* stmt,
                                         vector<util::ClangBaseWrapper>& v) const {
  if (DeclRefExpr* ref = dyn_cast<DeclRefExpr>(stmt))
    v.emplace_back(ref);
  if (DeclStmt* decl = dyn_cast<DeclStmt>(stmt)) {
    for (auto d = decl->decl_begin(); d != decl->decl_end(); ++d)
      v.emplace_back(cast<VarDecl>(*d));
  }
  for (auto c : stmt->children()) {
    if (c != nullptr) {
      findSubstitutions(c, v);
    }
  }
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
