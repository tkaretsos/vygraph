#ifndef VYGRAPH_TRANSLATOR_HH
#define VYGRAPH_TRANSLATOR_HH

#include <sstream>

#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"

namespace vy {

class Translator {
  clang::ASTContext& context;
  std::stringstream& outs;
  std::size_t indentLevel;
  std::string indentStr;

  void indent();
  void unindent();

  void beginFunction(const std::string&);
  void endFunction();

  void replaceAssignOp(std::string&) const;

public:
  Translator(std::stringstream&, clang::ASTContext&);

  void translateVarDecl(clang::VarDecl*);
  void translateFunction(clang::FunctionDecl*);
};

} // namespace vy

#endif // VYGRAPH_TRANSLATOR_HH
