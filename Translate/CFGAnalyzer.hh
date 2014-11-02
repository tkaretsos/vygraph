#ifndef VYGRAPH_CFGANALYZER_HH
#define VYGRAPH_CFGANALYZER_HH

#include <map>
#include <vector>

namespace clang {

class CFG;
class CFGBlock;

}

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

  void print() const;

private:
  unsigned int pcCounter = 0;
  std::map<unsigned int, std::vector<std::string>> locations;
  std::map<unsigned int, std::vector<std::string>::iterator> currLocs;
};

} // namespace vy

#endif //VYGRAPH_CFGANALYZER_HH
