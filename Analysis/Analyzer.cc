#include "Analyzer.hh"

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/Tooling.h"

#include "FunctionManager.hh"

namespace vy {

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

Analyzer::Analyzer(ClangTool& tool) : tool(tool) { }

void
Analyzer::analyze() {
  DeclarationMatcher funcDef = functionDecl(isDefinition()).bind("functionDef");

  MatchFinder defFinder;
  Analyzer::FunctionDefLocator defLocator;
  defFinder.addMatcher(funcDef, &defLocator);
  tool.run(newFrontendActionFactory(&defFinder));


  StatementMatcher simpleCall = callExpr(hasParent(compoundStmt()),
                                         hasDeclaration(funcDef)
                                        ).bind("simpleCall");

  StatementMatcher stmtWithCall = stmt(hasParent(compoundStmt()),
                                       hasDescendant(callExpr(hasDeclaration(funcDef))),
                                       unless(anyOf(ifStmt(), whileStmt(), forStmt()))
                                      ).bind("stmtWithCall");

  MatchFinder callFinder;
  Analyzer::FunctionCallLocator callLocator;
  callFinder.addMatcher(simpleCall, &callLocator);
  callFinder.addMatcher(stmtWithCall, &callLocator);
  tool.run(newFrontendActionFactory(&callFinder));
}

void
Analyzer::FunctionDefLocator::run(const MatchFinder::MatchResult& result) {
  if (auto function = result.Nodes.getNodeAs<FunctionDecl>("functionDef")) {
    if (!function->isMain())
      functionMgr.addUserFunction(function);
  }
}

void
Analyzer::FunctionCallLocator::run(const MatchFinder::MatchResult& result) {
  if (auto call = result.Nodes.getNodeAs<CallExpr>("simpleCall"))
    functionMgr.addCall(call, true);

  if (auto stmt = result.Nodes.getNodeAs<Stmt>("stmtWithCall")) {
    auto call = findCallInStmt(stmt);
    if (call != nullptr)
      functionMgr.addCall(call, false, stmt->getLocStart());
  }
}

const CallExpr*
Analyzer::FunctionCallLocator::findCallInStmt(const Stmt* stmt) {

  CallExpr* call = nullptr;
  if (call = dyn_cast<CallExpr>(const_cast<Stmt*>(stmt)))
    return call;

  if (!stmt->children().empty()) {
    for (auto c : stmt->children()) {
      call = const_cast<CallExpr*>(findCallInStmt(c));
      if (call != nullptr)
        break;
    }
  }

  return call;
}

} // namespace vy
