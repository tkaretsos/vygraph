#include "Analyzer.hh"
#include <iostream>
#include "clang/ASTMatchers/ASTMatchers.h"

namespace vy {

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

Analyzer::Analyzer(ClangTool& tool) : tool(tool) { }

void Analyzer::analyze() {
  Analyzer::FunctionDefLocator locator;
  MatchFinder finder;
  DeclarationMatcher functionDef = functionDecl(isDefinition()).bind("functionDef");
  finder.addMatcher(functionDef, &locator);
  tool.run(newFrontendActionFactory(&finder));

  Analyzer::FunctionCallLocator callLocator;
  MatchFinder callFinder;
  auto userFunc = functionDecl(isDefinition());

//   StatementMatcher funcAsArgument = callExpr(hasAnyArgument(declRefExpr(to(userFunc)))
//                                              ).bind("funcAsArgument");

  StatementMatcher simpleCall = callExpr(hasParent(compoundStmt()),
                                         hasDeclaration(userFunc)
                                         ).bind("simpleCall");

  StatementMatcher stmtWithCall = stmt(hasParent(compoundStmt()),
                                       hasDescendant(callExpr(hasDeclaration(userFunc)))
                                       ).bind("stmtWithCall");

//   callFinder.addMatcher(funcAsArgument, &callLocator);
  callFinder.addMatcher(simpleCall, &callLocator);
  callFinder.addMatcher(stmtWithCall, &callLocator);
  tool.run(newFrontendActionFactory(&callFinder));
}

void Analyzer::FunctionDefLocator::run(const MatchFinder::MatchResult& result) {
  if (auto function = result.Nodes.getNodeAs<FunctionDecl>("functionDef")) {
    if (!function->isMain()) {
      functionMgr.addUserFunction(function);
    }
  }
}

void Analyzer::FunctionCallLocator::run(const MatchFinder::MatchResult& result) {
//   if (auto call = result.Nodes.getNodeAs<CallExpr>("funcAsArgument")) {
//     for (auto i = call->arg_begin(); i != call->arg_end(); ++i) {
//       if (auto impCast = dyn_cast<ImplicitCastExpr>(*i)) {
//         if (auto declRef = dyn_cast<DeclRefExpr>(impCast->getSubExpr())) {
//           functionMgr.setDeleteSource(declRef->getNameInfo().getAsString(), false);
//         }
//       }
//     }
//   }

  if (auto call = result.Nodes.getNodeAs<CallExpr>("simpleCall")) {
    functionMgr.addCall(call, true);
  }

  if (auto stmt = result.Nodes.getNodeAs<Stmt>("stmtWithCall")) {
    const CallExpr* call = Analyzer::findCallInStmt(stmt);
    if (call != nullptr) {
      functionMgr.addCall(call, false, stmt->getLocStart());
    }
  }
}

const clang::CallExpr* Analyzer::findCallInStmt(const Stmt* stmt) {

  CallExpr* call = nullptr;
  if (call = dyn_cast<CallExpr>(const_cast<Stmt*>(stmt))) {
    return call;
  }

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
