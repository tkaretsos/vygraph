#include "Inliner.hh"

#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include "Analysis/FunctionManager.hh"
#include "Utility/ClangBaseWrapper.hh"
#include "Utility/Functions.hh"

namespace vy {

using namespace clang;
using namespace std;

Inliner::Inliner(const ASTContext& context, Rewriter& rewriter)
  : context(context), rewriter(rewriter)
{ }

void
Inliner::Init(CallExpr* call) {
  this->call = call;
  this->subMap = functionMgr.getVarSubs(call);

  string ext(util::random_alphanum());
  for (auto& sub : subMap)
    sub.second = sub.first + "_" + ext;
}

void
Inliner::Inline() {
  if (functionMgr.isSimpleCall(call))
    deleteCallText();

  if (call->getNumArgs() > 0)
    inlineArguments();

  inlineBody();
}

void
Inliner::inlineStmt(const Stmt* stmt, string& stmtStr) {
  if (isa<Expr>(stmt) || isa<DoStmt>(stmt))
    stmtStr.append(";");
  stmtStr.append("\n");
  rewriter.InsertText(functionMgr.getInsertLoc(call), stmtStr, true, true);
}

void
Inliner::inlineReturnStmt(string& stmtStr) {
  string replacement("(");
  // 7 is the length of the "return " string
  replacement.append(stmtStr.begin() + 7, stmtStr.end());
  replacement.push_back(')');
  rewriter.ReplaceText(call->getSourceRange(), replacement);
}

void
Inliner::inlineArguments() {
  auto param = call->getDirectCallee()->param_begin();
  for (auto arg = call->arg_begin(); arg != call->arg_end(); ++arg, ++param) {
    string insertStr((*param)->getOriginalType().getAsString() + " ");
    insertStr.append(subMap.at((*param)->getNameAsString()) + " = ");
    insertStr.append(rewriter.ConvertToString(*arg) + ";\n");
    rewriter.InsertText(functionMgr.getInsertLoc(call), insertStr, true, true);
  }
}

void
Inliner::inlineBody() {
  auto body = cast<CompoundStmt>(call->getDirectCallee()->getBody());
  for (auto stmt = body->body_begin(); stmt != body->body_end(); ++stmt) {
    string insertStr(util::RangeToStr((*stmt)->getSourceRange(), context));
    replaceVarsInString(*stmt, insertStr);
    if (isa<ReturnStmt>(*stmt) ) {
      if (!functionMgr.isSimpleCall(call))
        inlineReturnStmt(insertStr);
    } else
      inlineStmt(*stmt, insertStr);
  }
}

void
Inliner::deleteCallText() {
  rewriter.RemoveText(call->getSourceRange());
  rewriter.RemoveText(call->getLocStart(), 1);
}

void
Inliner::findSubstitutions(Stmt* stmt, vector<util::ClangBaseWrapper>& v) {
  if (DeclRefExpr* ref = dyn_cast<DeclRefExpr>(stmt))
    v.emplace_back(ref);

  if (DeclStmt* decl = dyn_cast<DeclStmt>(stmt)) {
    for (auto d = decl->decl_begin(); d != decl->decl_end(); ++d)
      v.emplace_back(cast<VarDecl>(*d));
  }

  for (auto c : stmt->children()) {
    if (c != nullptr)
      findSubstitutions(c, v);
  }
}

void
Inliner::replaceVarsInString(Stmt* stmt, string& s) {
  vector<util::ClangBaseWrapper> subs;
  findSubstitutions(stmt, subs);
  reverse(subs.begin(), subs.end());
  for (util::ClangBaseWrapper& sub : subs) {
    auto found = subMap.find(sub.getAsString(rewriter));
    if (found != subMap.end()) {
      auto offset = sub.getLocStart().getRawEncoding() -
                    stmt->getLocStart().getRawEncoding();
      auto begin = s.begin() + offset;
      s.replace(begin, begin + found->first.length(), found->second);
    }
  }
}

} // namespace vy
