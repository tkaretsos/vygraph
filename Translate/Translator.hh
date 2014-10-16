#ifndef VYGRAPH_TRANSLATOR_HH
#define VYGRAPH_TRANSLATOR_HH

#include <sstream>
#include <utility>

#include "clang/Analysis/CFG.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"

namespace vy {

class Translator {
  typedef std::pair<unsigned int, unsigned int> LocationPair;

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
  std::string getLocationStr(unsigned int, unsigned int) const;
  void insertStmt(const clang::Stmt*);
  void insertSequentialStmts(clang::CFGBlock::const_iterator,
                             clang::CFGBlock::const_iterator);
  void insertBranchCondTrue(const clang::Stmt*);
  void insertBranchCondFalse(const clang::Stmt*, const LocationPair&);
  void insertBranchTargetTrue(const clang::CFGBlock&);
  void insertBranchTargetFalse(const clang::CFGBlock&);
  void replaceAssignOp(std::string&) const;

  unsigned int getBranchExitID(const clang::CFGBlock&) const;

public:
  Translator(std::stringstream&, clang::ASTContext&);

  void translateVarDecl(const clang::VarDecl*);
  void translateFunction(clang::FunctionDecl*);
};

} // namespace vy

#endif // VYGRAPH_TRANSLATOR_HH
