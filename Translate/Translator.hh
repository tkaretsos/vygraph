#ifndef VYGRAPH_TRANSLATOR_HH
#define VYGRAPH_TRANSLATOR_HH

#include <memory>
#include <sstream>
#include <utility>

#include "clang/Analysis/AnalysisContext.h"
#include "clang/Analysis/CFG.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"

namespace vy {

class Translator {
public:
  typedef std::pair<unsigned int, unsigned int> LocationPair;

  Translator(std::stringstream&, clang::ASTContext&);

  void translateVarDecl(const clang::VarDecl*);
  void translateFunction(clang::FunctionDecl*);

private:
  clang::ASTContext& context;
  clang::AnalysisDeclContextManager analysisManager;
  std::unique_ptr<clang::AnalysisDeclContext> analysis;
  std::stringstream& outs;
  std::size_t indentLevel;
  std::string indentStr;
  unsigned int pcCounter;
  const unsigned int pcError = -1;

  clang::CFG* getCFG(const clang::FunctionDecl*);

  void indent();
  void unindent();

  void beginFunction(const std::string&);
  void endFunction();

  void insertStmt(const clang::Stmt*, const LocationPair* = nullptr);
  void insertSequentialStmts(clang::CFGBlock::const_iterator,
                             clang::CFGBlock::const_iterator);
  void insertSequentialStmts(clang::CFGBlock::const_iterator,
                             clang::CFGBlock::const_iterator,
                             unsigned int);
  void insertBranchCondTrue(const clang::Stmt*);
  void insertBranchCondFalse(const clang::Stmt*, const LocationPair&);
  void insertBranchTargetTrue(const clang::CFGBlock&);
  void insertBranchTargetTrue(const clang::CFGBlock&, unsigned int);
  void insertBranchTargetFalse(const clang::CFGBlock&);

  void replaceAssignOp(std::string&) const;
  LocationPair getLocation();
  unsigned int getBranchExitID(const clang::CFGBlock&) const;
  unsigned int getBranchEndLoc(const clang::CFGBlock&) const;
};

std::ostream& operator<<(std::ostream&, const Translator::LocationPair&);

} // namespace vy

#endif // VYGRAPH_TRANSLATOR_HH
