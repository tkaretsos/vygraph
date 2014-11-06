#ifndef VYGRAPH_TRANSLATOR_HH
#define VYGRAPH_TRANSLATOR_HH

#include "Translate/CFGAnalyzer.hh"

namespace clang {
class BinaryOperator;
class CallExpr;
}

namespace vy {

class Translator {
public:
  Translator(std::stringstream&, clang::ASTContext&);

  void translateVarDecl(const clang::VarDecl*);
  void translateFunction(clang::FunctionDecl*);

private:
  clang::ASTContext& context;
  std::stringstream& outs;
  std::size_t indentLevel;
  std::string indentStr;
  CFGAnalyzer analyzer;

  void indent();
  void unindent();

  void beginFunction(const clang::FunctionDecl*);
  void endFunction();

  void beginAtomic();
  void endAtomic();

  void writeCFG(const clang::CFGBlock&);
  void writeStatements(const clang::CFGBlock&);
  void writeStmt(const clang::CFGBlock&, const clang::Stmt*);
  void writeDefaultStmt(const clang::CFGBlock&, const clang::Stmt*);
  void writeBinaryOp(const clang::CFGBlock&, const clang::BinaryOperator*);
  void writeTerminatorFalse(const clang::CFGBlock&);
  void writeAssert(const clang::CFGBlock&, const clang::Expr*);
  void writeAssume(const clang::CFGBlock&, const clang::Stmt*);
  void writeCustomFunctionCall(const clang::CFGBlock&, const clang::CallExpr*);

  void replaceAssignOp(std::string&) const;
  void replaceEqualsOp(std::string&) const;

  bool hasElsePart(const clang::CFGBlock&) const;
};

} // namespace vy

#endif // VYGRAPH_TRANSLATOR_HH