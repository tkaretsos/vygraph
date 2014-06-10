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
  DeclarationMatcher matcher = functionDecl(isDefinition()).bind("functionDef");
  finder.addMatcher(matcher, &locator);
  tool.run(newFrontendActionFactory(&finder));

  Analyzer::FunctionCallLocator callLocator;
  MatchFinder callFinder;
  StatementMatcher stmtMatcher =
    callExpr(hasAnyArgument(declRefExpr(to(functionDecl(isDefinition()))))).bind("funcAsArgument");
  callFinder.addMatcher(stmtMatcher, &callLocator);
  tool.run(newFrontendActionFactory(&callFinder));

  functionMgr.print();
}

void Analyzer::FunctionDefLocator::run(const MatchFinder::MatchResult& result) {
  if (auto function = result.Nodes.getNodeAs<FunctionDecl>("functionDef")) {
    if (!function->isMain()) {
      functionMgr.addUserFunction(function);
    }
  }
}

void Analyzer::FunctionCallLocator::run(const MatchFinder::MatchResult& result) {
  if (auto call = result.Nodes.getNodeAs<CallExpr>("funcAsArgument")) {
    for (auto i = call->arg_begin(); i != call->arg_end(); ++i) {
      if (auto impCast = dyn_cast<ImplicitCastExpr>(*i)) {
        if (auto declRef = dyn_cast<DeclRefExpr>(impCast->getSubExpr())) {
          functionMgr.setDeleteSource(declRef->getNameInfo().getAsString(), false);
        }
      }
    }
  }
}

}
