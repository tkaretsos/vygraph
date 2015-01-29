#ifndef VYGRAPH_INLINER_HH
#define VYGRAPH_INLINER_HH

#include <map>
#include <vector>

namespace clang {

class ASTContext;
class CallExpr;
class Rewriter;
class SourceLocation;
class Stmt;

}

namespace vy {

namespace util {

class ClangBaseWrapper;

}

/**
 * @brief Class that performs the actual inlining of a user defined function
 *
 */
class Inliner {
public:
  Inliner(const clang::ASTContext&, clang::Rewriter&);

  /**
   * @brief Perform the required initialization for the given function call
   *
   * Must be called before the call to the Inliner::Inline method
   *
   * @param call The call to the function
   */
  void Init(clang::CallExpr* call);

  /**
   * @brief Perform the inlining
   */
  void Inline();

private:
  const clang::ASTContext& context;
  clang::Rewriter& rewriter;
  clang::CallExpr* call;

  /**
   * @brief Auxiliary `std::map` that holds the variable name substitutions for
   * this function call.
   */
  std::map<std::string, std::string> subMap;

  /**
   * @brief Inline the given statement
   *
   * @param stmt The `clang:Stmt` that represents the C statement
   * @param stmtStr The statement in the form of a `std::string`
   */
  void inlineStmt(const clang::Stmt* stmt, std::string& stmtStr);

  /**
   * @brief Inline a return statement
   *
   * @param stmtStr The return statement in the form of a `std::string`
   */
  void inlineReturnStmt(std::string& stmtStr);

  /**
   * @brief Inlines the arguments of the function
   */
  void inlineArguments();

  /**
   * @brief Inlines the function body
   */
  void inlineBody();

  /**
   * @brief Deletes the call text after the inlining is completed
   */
  void deleteCallText();

  /**
   * @brief Renames the variables of the function in order to be inlined and not
   * create any conflicts or multiple declarations with other possible inlined
   * calls to the same function.
   *
   * @param stmt The statement that contains the variables
   * @param s The string that will contain the replacements
   */
  void replaceVarsInString(clang::Stmt* stmt, std::string& s);

  /**
   * @brief Recursively locates and stores in a container the locations of the
   * variables, that need to be renamed, inside the C statement
   *
   * @param stmt The statement that contains the variables
   * @param v The container that will hold the variables that need to be renamed
   */
  void findSubstitutions(clang::Stmt* stmt, std::vector<util::ClangBaseWrapper>& v);

};

} // namespace vy

#endif // VYGRAPH_INLINER_HH

/** @file */
