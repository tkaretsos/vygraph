#ifndef VYGRAPH_POSTDOMINATORTREE_HH
#define VYGRAPH_POSTDOMINATORTREE_HH

namespace clang {

class AnalysisDeclContext;
class CFGBlock;

}

namespace llvm {

template <typename T> class DominatorTreeBase;

}

namespace vy {
namespace util {

/**
 * @brief Based on LLVM's DominatorTreeBase class.
 * @see http://llvm.org/doxygen/classllvm_1_1DominatorTreeBase.html
 */
class PostDominatorTree {
public:
  PostDominatorTree();
  ~PostDominatorTree();

  void buildPostDomTree(clang::AnalysisDeclContext&) const;
  bool dominates(const clang::CFGBlock*, const clang::CFGBlock*) const;
  clang::CFGBlock* findNearestCommonDominator(clang::CFGBlock*,
                                              clang::CFGBlock*) const;

  void releaseMemory() const;

private:
  llvm::DominatorTreeBase<clang::CFGBlock>* DT;

};

} // namespace util
} // namespace vy

#endif // VYGRAPH_POSTDOMINATORTREE_HH

/** @file */
