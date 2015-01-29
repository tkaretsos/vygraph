#ifndef VYGRAPH_TRANSLATOR_HH
#define VYGRAPH_TRANSLATOR_HH

#include "Translate/CFGAnalyzer.hh"

namespace clang {

class BinaryOperator;
class CallExpr;

}

namespace vy {

/**
 * @brief Performs the source to source translation from C to Multimono
 */
class Translator {
public:
  /**
   * @brief Constructor
   *
   * @param sstream The stream to store the result
   * @param context The Clang context
   */
  Translator(std::stringstream& sstream, clang::ASTContext& context);

  /**
   * @brief Translate variable declarations
   * @param varDecl Pointer to the variable declaration
   */
  void translateVarDecl(const clang::VarDecl* varDecl);

  /**
   * @brief Translate a C function
   * @param funcDecl Pointer to the function declaration
   */
  void translateFunction(clang::FunctionDecl* funcDecl);

private:
  clang::ASTContext& context;

  /**
   * @brief Stream where the result of the translation is stored.
   */
  std::stringstream& outs;

  /**
   * @brief The indentation level of the output
   */
  std::size_t indentLevel;

  /**
   * @brief Contains spaces the number of which is given by the indentation level times
   * the number of spaces used for each indentation (default 2).
   */
  std::string indentStr;

  CFGAnalyzer analyzer;

  /**
   * @brief Increase the indentation level
   * @sa Translator::indentLevel
   * @sa Translator::indentStr
   */
  void indent();

  /**
   * @brief Decrease the indentation level
   * @sa Translator::indentLevel
   * @sa Translator::indentStr
   */
  void unindent();

  /**
   * @brief Open the scope of the function being translated
   *
   * @param funcDecl Pointer to the function declaration
   */
  void beginFunction(const clang::FunctionDecl* funcDecl);

  /**
   * @brief Close the scope of the function being translated
   */
  void endFunction();

  /**
   * @brief Begin a Multimono atomic region
   */
  void beginAtomic();

  /**
   * @brief Close a Multimono atomic region
   */
  void endAtomic();

  /**
   * @brief The recursive algorithm that walks the CFG and translates a C function.
   * @param block The first block of the CFG (the one that the entry points to)
   */
  void writeCFG(const clang::CFGBlock& block);

  /**
   * @brief Translates the statements that the given CFG block contains
   * @param block A block of the CFG
   */
  void writeStatements(const clang::CFGBlock& block);

  /**
   * @brief Translates a C statement to Multimono
   * Works as an aggregator of the various special cases of C statements.
   * @param block The block of the CFG that contains the statement
   * @param stmt Pointer to the statement
   * @sa Translator::writeDefaultStmt
   * @sa Translator::writeBinaryOp
   * @sa Translator::writeCustomFunctionCall
   */
  void writeStmt(const clang::CFGBlock& block, const clang::Stmt* stmt);

  /**
   * @brief Translates the statements that are not specially handled by another
   * method of this class.
   * @param block The block that contains the statement
   * @param stmt Pointer to the statement
   * @sa Translator::writeStmt
   */
  void writeDefaultStmt(const clang::CFGBlock& block, const clang::Stmt* stmt);

  /**
   * @brief Translates the statements that are of `Stmt::BinaryOperatorClass`
   * @param block The block that contains the statement
   * @param binOp Pointer to the statement
   * @see http://clang.llvm.org/doxygen/classclang_1_1BinaryOperator.html
   */
  void writeBinaryOp(const clang::CFGBlock& block,
                     const clang::BinaryOperator* binOp);

  /**
   * @brief Writes an extra Multimono statement that transitions from a condition to
   * the its else part
   * @param block The CFG block that contains the terminator condition
   */
  void writeTerminatorFalse(const clang::CFGBlock& block);

  /**
   * @brief Translates a C `assert` statement to Multimono's `assume`
   * @param block The CFG block that contains the statement
   * @param expr Pointer to the expression
   */
  void writeAssert(const clang::CFGBlock& block, const clang::Expr* expr);

  /**
   * @brief Writes an `assume` statement in Multimono
   * @param block The block of the CFG that is being visited
   * @param condition The condition statement to write in the `assume`
   */
  void writeAssume(const clang::CFGBlock& block, const clang::Stmt* condition);

  /**
   * @brief Handles all the special functions declared in the custom C header
   * `multimono.h`
   * @param block The block that contains the statement
   * @param call Pointer to the call of the special function
   */
  void writeCustomFunctionCall(const clang::CFGBlock& block,
                               const clang::CallExpr* call);

  /**
   * @brief Replaces the C `=` operator with Multimono's `:=`
   * @param expr A `std::string` that holds the expression with the `=` operator
   */
  void replaceAssignOp(std::string& expr) const;

  /**
   * @brief Replaces the C `==` operator with Multimono's `=`
   * @param expr A `std::string` that holds the expression with the `==` operator
   */
  void replaceEqualsOp(std::string& expr) const;
};

} // namespace vy

#endif // VYGRAPH_TRANSLATOR_HH

/** @file */
