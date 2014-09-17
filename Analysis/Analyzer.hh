#ifndef VYGRAPH_ANALYZER_HH
#define VYGRAPH_ANALYZER_HH

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"

#include "FunctionManager.hh"

namespace vy {

using clang::ast_matchers::MatchFinder;
using clang::tooling::ClangTool;

class Analyzer {
  ClangTool& tool;
  class FunctionDefLocator : public MatchFinder::MatchCallback {
    virtual void run(const MatchFinder::MatchResult&);
  };

  class FunctionCallLocator : public MatchFinder::MatchCallback {
    virtual void run(const MatchFinder::MatchResult&);
  };

  static const clang::CallExpr* findCallInStmt(const clang::Stmt* stmt);

public:
  Analyzer(ClangTool&);
  void analyze();
};

}

#endif // VYGRAPH_ANALYZER_HH
