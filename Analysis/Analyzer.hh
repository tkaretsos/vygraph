#ifndef VYGRAPH_ANALYZER_HH
#define VYGRAPH_ANALYZER_HH

#include "clang/ASTMatchers/ASTMatchFinder.h"

namespace clang {
  namespace tooling {
    class ClangTool;
  }
}

namespace vy {

class Analyzer {
public:
  Analyzer(clang::tooling::ClangTool&);

  void analyze();

private:
  clang::tooling::ClangTool& tool;

  class FunctionDefLocator : public clang::ast_matchers::MatchFinder::MatchCallback {
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult&);
  };

  class FunctionCallLocator : public clang::ast_matchers::MatchFinder::MatchCallback {
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult&);
    const clang::CallExpr* findCallInStmt(const clang::Stmt*);
  };
};

} // namespace vy

#endif // VYGRAPH_ANALYZER_HH
