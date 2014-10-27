#ifndef VYGRAPH_TRANSLATOR_HH
#define VYGRAPH_TRANSLATOR_HH

#include <memory>
#include <sstream>
#include <utility>

#include "clang/Analysis/Analyses/Dominators.h"
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
  clang::DominatorTree domTree;
  clang::CFG* cfg;
  std::unique_ptr<clang::AnalysisDeclContext> analysis;
  std::stringstream& outs;
  std::size_t indentLevel;
  std::string indentStr;
  unsigned int pcCounter;
  const unsigned int pcError = -1;

  void indent();
  void unindent();

  void beginFunction(const clang::FunctionDecl*);
  void endFunction();

  void insertSubCFG(const clang::CFGBlock&,
                    const unsigned int* = nullptr,
                    const unsigned int* = nullptr);

  void insertStmt(const clang::Stmt*, const LocationPair* = nullptr);
  void insertSequentialStmts(clang::CFGBlock::const_iterator,
                             clang::CFGBlock::const_iterator);
  void insertSequentialStmts(clang::CFGBlock::const_iterator,
                             clang::CFGBlock::const_iterator,
                             const unsigned int*,
                             const unsigned int*);

  void insertBranchCondTrue(const clang::Stmt*);
  void insertBranchCondFalse(const clang::Stmt*, const LocationPair&);

  void replaceAssignOp(std::string&) const;
  LocationPair getLocation();
  const clang::CFGBlock& getBranchExitBlock(const clang::CFGBlock&) const;

  bool hasElsePart(const clang::CFGBlock&) const;
};

std::ostream& operator<<(std::ostream&, const Translator::LocationPair&);

} // namespace vy

#endif // VYGRAPH_TRANSLATOR_HH
