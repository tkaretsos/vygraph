#ifndef VYGRAPH_TRANSLATOR_HH
#define VYGRAPH_TRANSLATOR_HH

#include "Translate/CFGAnalyzer.hh"

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
  void writeTerminatorFalse(const clang::CFGBlock&);
  void writeAssert(const clang::CFGBlock&, const clang::Expr*);
  void writeAssume(const clang::CFGBlock&, const clang::Stmt*);

  void replaceAssignOp(std::string&) const;

  bool hasElsePart(const clang::CFGBlock&) const;
  std::string getLocString(const clang::CFGBlock&, bool = false);
};

} // namespace vy

#endif // VYGRAPH_TRANSLATOR_HH
