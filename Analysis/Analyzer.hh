#ifndef VYGRAPH_ANALYZER_HH
#define VYGRAPH_ANALYZER_HH

#include "clang/ASTMatchers/ASTMatchFinder.h"

namespace clang {
namespace tooling {

class ClangTool;

}
}

namespace vy {

/**
 * @brief Class that is used to locate the user defined functions and their
 * respective calls from the main function in the translation unit.
 *
 * @see http://clang.llvm.org/docs/LibASTMatchersTutorial.html
 * @see http://clang.llvm.org/docs/LibASTMatchersReference.html
 */
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

    /**
     * @brief A helper function that locates the call in a more complex statement
     * For example:
     * <code>
     * <br> ... C code ...
     * <br> x = 5 + bar();
     * <br> ... C code ...
     *
     * @param stmt The `clang:Stmt` that contains the call
     * @return A pointer to the `clang::CallExpr`
     */
    const clang::CallExpr* findCallInStmt(const clang::Stmt* stmt);
  };

};

} // namespace vy

#endif // VYGRAPH_ANALYZER_HH

/** @file */
