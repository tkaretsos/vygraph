#include "Analyzer.hh"
#include <iostream>
#include "clang/ASTMatchers/ASTMatchers.h"

namespace vy {

using namespace clang::tooling;
using namespace clang::ast_matchers;

Analyzer::Analyzer(ClangTool& tool) : tool(tool) { }

void Analyzer::FuntionLocator::run(const MatchFinder::MatchResult& result) {
  std::cout << "hello" << std::endl;
}

void Analyzer::analyze() {
  Analyzer::FuntionLocator locator;
  MatchFinder finder;
  StatementMatcher matcher = callExpr().bind("callExpr");
  finder.addMatcher(matcher, &locator);
  tool.run(newFrontendActionFactory(&finder));
}

}
