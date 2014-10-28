#include "CFGAnalyzer.hh"

#include <iostream>

namespace vy {

using namespace clang;
using namespace std;

void
CFGAnalyzer::analyze(const CFG* cfg) {
  for (auto block = cfg->rbegin(); block != cfg->rend(); ++block) {
    auto& v = locations[(*block)->getBlockID()];
    for (auto elem = (*block)->begin(); elem != (*block)->end(); ++elem) {
      if (auto stmt = elem->getAs<CFGStmt>()) {
        if (!isa<DeclStmt>(stmt->getStmt()) && !isa<ReturnStmt>(stmt->getStmt()))
          v.push_back("pc" + to_string(pcCounter++));
      }
    }
    currLocs[(*block)->getBlockID()] = v.begin();
  }

  locations[cfg->getExit().getBlockID()].push_back("ext");
}

const string&
CFGAnalyzer::getFirstLoc(const CFGBlock& block) const {
  return locations.at(block.getBlockID()).front();
}

const string&
CFGAnalyzer::getLastLoc(const CFGBlock& block) const {
  return locations.at(block.getBlockID()).back();
}

const string&
CFGAnalyzer::getCurrentLoc(const CFGBlock& block) const {
  return *currLocs.at(block.getBlockID());
}

const string&
CFGAnalyzer::getNextLoc(const CFGBlock& block) {
  return *++currLocs.at(block.getBlockID());
}

bool
CFGAnalyzer::hasNextLoc(const CFGBlock& block) const {
  return currLocs.at(block.getBlockID()) + 1 != locations.at(block.getBlockID()).end();
}

}
