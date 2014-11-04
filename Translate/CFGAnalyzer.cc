#include "CFGAnalyzer.hh"

#include <iostream>

#include "clang/Analysis/CFG.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Stmt.h"

namespace vy {

using namespace clang;
using namespace std;

void
CFGAnalyzer::analyze(const clang::FunctionDecl* funcDecl) {

  init(funcDecl);

  for (auto block = cfg->rbegin(); block != cfg->rend(); ++block) {
    auto& v = locations[(*block)->getBlockID()];
    for (auto elem = (*block)->begin(); elem != (*block)->end(); ++elem) {
      if (auto stmt = elem->getAs<CFGStmt>()) {
        switch (stmt->getStmt()->getStmtClass()) {
          default:
            v.push_back("pc" + to_string(pcCounter++));
            break;

          case Stmt::CallExprClass: {
            auto call = cast<CallExpr>(stmt->getStmt());
            if (call->getDirectCallee()->getNameAsString() == "vy_atomic_begin") {
              atomicBegin(**block);
              break;
            }
            if (call->getDirectCallee()->getNameAsString() == "vy_atomic_end") {
              atomicEnd(**block);
              break;
            }
            v.push_back("pc" + to_string(pcCounter++));
            break;
          }

          case Stmt::DeclStmtClass:
          case Stmt::ReturnStmtClass:
            break;
        }
      }
    }
    currLocs[(*block)->getBlockID()] = v.begin();
  }

  locations[cfg->getExit().getBlockID()].push_back("ext");

  if (!atomicStack.empty())
    cout << "stack not empty" << endl;
}

void
CFGAnalyzer::finalize() {
  domTree.releaseMemory();
  postDomTree.releaseMemory();
}

const CFG*
CFGAnalyzer::getCFG() const {
  return cfg;
}

const DominatorTree&
CFGAnalyzer::getDomTree() const {
  return domTree;
}

const CFGBlock*
CFGAnalyzer::findFirstPostDominator(const CFGBlock& block) const {
  return postDomTree.findNearestCommonDominator(*block.succ_begin(),
                                                *block.succ_rbegin());
}

const string&
CFGAnalyzer::getFirstAvailableLoc(const CFGBlock& block) const {
  auto nextBlock = &block;
  while (locations.at(nextBlock->getBlockID()).size() == 0)
    nextBlock = *nextBlock->succ_begin();
  return locations.at(nextBlock->getBlockID()).front();
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

string
CFGAnalyzer::getLocString(const CFGBlock& block, bool toErr) {
  string ret(getCurrentLoc(block) + " -> ");
  if (toErr) {
    ret.append("err");
  } else {
    if (hasNextLoc(block)) {
      ret.append(getNextLoc(block));
    } else {
      ret.append(getFirstAvailableLoc(**block.succ_begin()));
    }
  }
  ret.append(": ");
  return ret;
}

string
CFGAnalyzer::getTerminatorFalseLoc(const CFGBlock& block) {
  string loc(locations.at(block.getBlockID()).back() + " -> ");
  loc.append(getFirstAvailableLoc(**block.succ_rbegin()) + ": ");
  return loc;
}

void
CFGAnalyzer::print() const {
  for (auto& p : locations) {
    cout << p.first << " -> ";
    for (auto v : p.second)
      cout << v << " ";
    cout << endl;
  }
}

void CFGAnalyzer::init(const clang::FunctionDecl* funcDecl) {
  pcCounter = 0;
  analysisContext.reset(new AnalysisDeclContext(&analysisManager, funcDecl));
  cfg = analysisContext->getCFG();
  domTree.buildDominatorTree(*analysisContext);
  postDomTree.buildPostDomTree(*analysisContext);
}

void
CFGAnalyzer::atomicBegin(const CFGBlock& block) {
  atomicStack.push(&block);
}

void
CFGAnalyzer::atomicEnd(const CFGBlock& block) {
  if (atomicStack.empty()) {
    cout << "atomic stack empty" << endl;
    return;
  }

  if (domTree.dominates(atomicStack.top(), &block) &&
      postDomTree.dominates(&block, atomicStack.top())) {
    atomicStack.pop();
  } else {
    cout << "no mutual domination" << endl;
  }
}

}
