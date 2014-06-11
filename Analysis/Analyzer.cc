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
  StatementMatcher funcAsArgument = callExpr(hasAnyArgument(declRefExpr(to(userFunc)))
                                             ).bind("funcAsArgument");
  StatementMatcher simpleCall = callExpr(hasParent(compoundStmt()),
                                         hasDeclaration(userFunc)
                                         ).bind("simpleCall");
  StatementMatcher callInExpr = callExpr(unless(hasParent(compoundStmt())),
                                         hasDeclaration(userFunc)
                                         ).bind("callInExpr");
  callFinder.addMatcher(funcAsArgument, &callLocator);
  callFinder.addMatcher(simpleCall, &callLocator);
  callFinder.addMatcher(callInExpr, &callLocator);
  tool.run(newFrontendActionFactory(&callFinder));

  functionMgr.print();
}

void Analyzer::FunctionDefLocator::run(const MatchFinder::MatchResult& result) {
  if (const FunctionDecl* function = result.Nodes.getNodeAs<FunctionDecl>("functionDef")) {
    if (!function->isMain()) {
      functionMgr.addUserFunction(function);
    }
  }
}

void Analyzer::FunctionCallLocator::run(const MatchFinder::MatchResult& result) {
  if (const CallExpr* call = result.Nodes.getNodeAs<CallExpr>("funcAsArgument")) {
    for (auto i = call->arg_begin(); i != call->arg_end(); ++i) {
      if (auto impCast = dyn_cast<ImplicitCastExpr>(*i)) {
        if (auto declRef = dyn_cast<DeclRefExpr>(impCast->getSubExpr())) {
          functionMgr.setDeleteSource(declRef->getNameInfo().getAsString(), false);
        }
      }
    }
  }

  if (const CallExpr* call = result.Nodes.getNodeAs<CallExpr>("simpleCall")) {
    functionMgr.addCall(call, true);
  }

  if (const CallExpr* call = result.Nodes.getNodeAs<CallExpr>("callInExpr")) {
    functionMgr.addCall(call, false);
  }
}

}
