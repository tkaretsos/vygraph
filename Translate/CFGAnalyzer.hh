#ifndef VYGRAPH_CFGANALYZER_HH
#define VYGRAPH_CFGANALYZER_HH

#include <map>
#include <stack>
#include <vector>

#include "clang/Analysis/Analyses/Dominators.h"

#include "Utility/PostDominatorTree.hh"

namespace vy {

class CFGAnalyzer {
public:
  void analyze(const clang::FunctionDecl*);
  void finalize();

  const clang::CFG* getCFG() const;
  const clang::DominatorTree& getDomTree() const;
  const clang::CFGBlock* findFirstPostDominator(const clang::CFGBlock&) const;

  std::string getLocString(const clang::CFGBlock&, bool = false);
  std::string getTerminatorFalseLoc(const clang::CFGBlock&);

  void print() const;

private:
  unsigned int pcCounter = 0;
  std::map<unsigned int, std::vector<std::string>> locations;
  std::map<unsigned int, std::vector<std::string>::iterator> currLocs;
  std::stack<const clang::CFGBlock*> atomicStack;

  clang::CFG* cfg;
  clang::AnalysisDeclContextManager analysisManager;
  std::unique_ptr<clang::AnalysisDeclContext> analysisContext;
  clang::DominatorTree domTree;
  util::PostDominatorTree postDomTree;

  void init(const clang::FunctionDecl*);

  void atomicBegin(const clang::CFGBlock&);
  void atomicEnd(const clang::CFGBlock&);

  const std::string& getFirstAvailableLoc(const clang::CFGBlock&) const;
  const std::string& getCurrentLoc(const clang::CFGBlock&) const;
  const std::string& getNextLoc(const clang::CFGBlock&);
  bool hasNextLoc(const clang::CFGBlock&) const;
};

} // namespace vy

#endif //VYGRAPH_CFGANALYZER_HH
