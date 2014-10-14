#ifndef VYGRAPH_TRANSLATOR_HH
#define VYGRAPH_TRANSLATOR_HH

#include <sstream>

#include "clang/Analysis/CFG.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"

namespace vy {

class Translator {
  clang::ASTContext& context;
  std::stringstream& outs;
  std::size_t indentLevel;
  std::string indentStr;
  unsigned int pcCounter;
  const unsigned int pcError = -1;

  void indent();
  void unindent();

  void beginFunction(const std::string&);
  void endFunction();

  void insertLocationStr();
  void insertSequentialStmts(clang::CFGBlock::const_iterator,
                             clang::CFGBlock::const_iterator);
  void replaceAssignOp(std::string&) const;

public:
  Translator(std::stringstream&, clang::ASTContext&);

  void translateVarDecl(const clang::VarDecl*);
  void translateFunction(clang::FunctionDecl*);
};

} // namespace vy

#endif // VYGRAPH_TRANSLATOR_HH
