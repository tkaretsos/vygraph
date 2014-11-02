#ifndef VYGRAPH_TRANSLATOR_HH
#define VYGRAPH_TRANSLATOR_HH

#include "clang/Analysis/Analyses/Dominators.h"

#include "Translate/CFGAnalyzer.hh"
#include "Utility/PostDominatorTree.hh"

namespace vy {

class Translator {
public:
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
  CFGAnalyzer analyzer;
  util::PostDominatorTree postDomTree;

  void indent();
  void unindent();

  void beginFunction(const clang::FunctionDecl*);
  void endFunction();

  void insertSubCFG(const clang::CFGBlock&);

  void insertSequentialStmts(const clang::CFGBlock&);

  void insertTerminatorFalse(const clang::CFGBlock&);

  void replaceAssignOp(std::string&) const;
  const clang::CFGBlock& getPostDominator(const clang::CFGBlock&) const;
  clang::CFGBlock* getFirstPostDominator(const clang::CFGBlock&) const;

  bool hasTerminator(const clang::CFGBlock&) const;
  bool hasElsePart(const clang::CFGBlock&) const;
  std::string getLocString(const clang::CFGBlock&);
};

} // namespace vy

#endif // VYGRAPH_TRANSLATOR_HH
