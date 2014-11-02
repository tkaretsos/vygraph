#include "PostDominatorTree.hh"

#include "clang/Analysis/AnalysisContext.h"
#include "llvm/Analysis/Dominators.h"

#include <iostream>

namespace vy {
namespace util {

PostDominatorTree::PostDominatorTree() {
  DT = new llvm::DominatorTreeBase<clang::CFGBlock>(true);
}

PostDominatorTree::~PostDominatorTree() {
  delete DT;
}

void
PostDominatorTree::buildPostDomTree(clang::AnalysisDeclContext& AC) const {
  DT->recalculate(*AC.getCFG());
}

bool
PostDominatorTree::dominates(const clang::CFGBlock* A,
                             const clang::CFGBlock* B) const {
  return DT->dominates(A, B);
}

clang::CFGBlock*
PostDominatorTree::findNearestCommonDominator(clang::CFGBlock* A,
                                              clang::CFGBlock* B) const {
  return DT->findNearestCommonDominator(A, B);
}

} // namespace util
} // namespace vy
