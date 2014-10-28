#ifndef VYGRAPH_CFGANALYZER_HH
#define VYGRAPH_CFGANALYZER_HH

#include <map>
#include <vector>

#include "clang/Analysis/CFG.h"

namespace vy {

class CFGAnalyzer {
public:
  void analyze(const clang::CFG*);

  const std::string& getFirstLoc(const clang::CFGBlock&) const;
  const std::string& getFirstAvailableLoc(const clang::CFGBlock&) const;
  const std::string& getLastLoc(const clang::CFGBlock&) const;
  const std::string& getCurrentLoc(const clang::CFGBlock&) const;
  const std::string& getNextLoc(const clang::CFGBlock&);
  bool hasNextLoc(const clang::CFGBlock&) const;

  void setInsertedBlock(const clang::CFGBlock&, bool = true);
  bool isInserted(const clang::CFGBlock&) const;

private:
  unsigned int pcCounter = 0;
  std::map<unsigned int, std::vector<std::string>> locations;
  std::map<unsigned int, std::vector<std::string>::iterator> currLocs;
  std::map<unsigned int, bool> insertedBlocks;
};

} // namespace vy

#endif //VYGRAPH_CFGANALYZER_HH
