#ifndef VYGRAPH_CFGANALYZER_HH
#define VYGRAPH_CFGANALYZER_HH

#include <map>
#include <stack>
#include <vector>

#include "clang/Analysis/Analyses/Dominators.h"

#include "Utility/PostDominatorTree.hh"

namespace vy {

/**
 * @brief Performs the necessary procedures to prepare for the translation from
 * C to Multimono
 */
class CFGAnalyzer {
public:
  /**
   * @brief Analyzes the function that is about to be translated.
   *
   * Assigns the required location numbers to the C statements of the function,
   * when this is necessary. At the same time, it checks for the correctness of
   * the atomic regions.
   *
   * @param funcDecl The reference to the function
   */
  void analyze(const clang::FunctionDecl* funcDecl);

  /**
   * @brief Clears the containers of this object and releases the memory.
   */
  void finalize();

  /**
   * @return A pointer to the control flow graph of the function as Clang generates
   * it.
   * @see http://clang.llvm.org/doxygen/classclang_1_1CFG.html
   */
  const clang::CFG* getCFG() const;

  /**
   * @return A reference to the dominator tree of the CFG.
   * @see http://clang.llvm.org/doxygen/classclang_1_1DominatorTree.html
   */
  const clang::DominatorTree& getDomTree() const;

  /**
   * @return A reference to the post dominator tree of the CFG.
   * @see vy::util::PostDominatorTree
   */
  const util::PostDominatorTree& getPostDomTree() const;

  /**
   * @param block A block of the CFG to find its closest post dominator
   * @return A pointer to the closest post dominator block in the CFG.
   */
  const clang::CFGBlock* findFirstPostDominator(const clang::CFGBlock& block) const;

  /**
   * @brief Creates a `std::string` that contains the location transition depending
   * on the location of the given block.
   *
   * @param block A block of the CFG
   * @param toErr If `true` the next location is the error state. Defaults to `false`.
   * @return A `std::string` in the form of `pc2 -> pc3:`. If `toErr` is `true`
   * then the string will have the form `pc2 -> err:`.
   */
  std::string getLocString(const clang::CFGBlock& block, bool toErr = false);

  /**
   * @brief Creates a `std::string` that contains the location transition for the else
   * part of an if statement
   *
   * @param block The current block of the CFG
   * @return A `std::string` in the form of `pc2 -> pc5:`
   */
  std::string getTerminatorFalseLoc(const clang::CFGBlock& block);

  /**
   * @param block A block of the CFG
   * @return A `std::string` in the form of `pc2` that contains the location of
   * the first statement in the block
   */
  const std::string& getFirstLoc(const clang::CFGBlock& block);

  /**
   * @param block A block of the CFG
   * @return A `std::string` in the form of `pc2` that contains the location of
   * the last statement in the block
   */
  const std::string& getLastLoc(const clang::CFGBlock& block);

private:
  /**
   * @brief Keeps track of the location positions. Each statement has a unique number
   */
  unsigned int pcCounter = 0;

  /**
   * @brief Maps each block of the CFG, using its unique ID, to a container that holds
   * all the locations of its statements
   */
  std::map<unsigned int, std::vector<std::string>> locations;

  /**
   * @brief Keeps track of the last visited statement in each block of the CFG
   */
  std::map<unsigned int, std::vector<std::string>::iterator> currLocs;

  /**
   * @brief Used to check the correctness of the atomic statements defined by the
   * user
   */
  std::stack<const clang::CFGBlock*> atomicStack;

  /**
   * @brief The CFG of the function under processing
   */
  clang::CFG* cfg;

  /**
   * @brief Required to construct the CFG
   * @see http://clang.llvm.org/doxygen/classclang_1_1AnalysisDeclContextManager.html
   * @see http://clang.llvm.org/doxygen/classclang_1_1AnalysisDeclContext.html
   * @see http://clang.llvm.org/doxygen/classclang_1_1AnalysisDeclContext.html#a8a3aa357f5d09fa5c71e952f961a5bca
   */
  clang::AnalysisDeclContextManager analysisManager;

  /**
   * @brief Required to construct the CFG
   * @see http://clang.llvm.org/doxygen/classclang_1_1AnalysisDeclContextManager.html
   * @see http://clang.llvm.org/doxygen/classclang_1_1AnalysisDeclContext.html
   * @see http://clang.llvm.org/doxygen/classclang_1_1AnalysisDeclContext.html#a8a3aa357f5d09fa5c71e952f961a5bca
   */
  std::unique_ptr<clang::AnalysisDeclContext> analysisContext;

  clang::DominatorTree domTree;
  util::PostDominatorTree postDomTree;

  /**
   * @brief Performs the initialization of this object
   * @param funcDecl The function that is being translated
   */
  void init(const clang::FunctionDecl* funcDecl);

  /**
   * @brief Marks the beginning of an atomic region
   * Needed for the atomic region correctness check.
   * @param block The block that the atomic region begins
   */
  void atomicBegin(const clang::CFGBlock& block);

  /**
   * @brief Marks the end of an atomic region
   * Needed for the atomic region correctness check.
   * @param block The block that the atomic region ends
   */
  void atomicEnd(const clang::CFGBlock& block);

  /**
   * @brief Get the next available location from the current location of the given
   * block.
   * It is possible that the next location exists in another block of the CFG
   *
   * @param block A block of the CFG
   * @return A `std::string` in the form of `pc3`
   */
  const std::string& getFirstAvailableLoc(const clang::CFGBlock& block) const;

  /**
   * @brief Get the current location of the given block
   *
   * @param block A block of the CFG
   * @return A `std::string` in the form of `pc1`
   */
  const std::string& getCurrentLoc(const clang::CFGBlock& block) const;

  /**
   * @brief Get the next location within the block. It does not check whether there
   * is next location or not. Must be used in conjunction with CFGAnalyzer::hasNextLoc
   *
   * @param block A block of the CFG
   * @return A `std::string` in the form of `pc2`
   * @see CFGAnalyzer::hasNextLoc
   */
  const std::string& getNextLoc(const clang::CFGBlock& block);

  /**
   * @brief Checks whether the block has reached the last location or not
   *
   * @param block A block of the CFG
   * @return `true` if there is at least one more location (statement) in the block. `false` otherwise.
   */
  bool hasNextLoc(const clang::CFGBlock& block) const;

};

} // namespace vy

#endif //VYGRAPH_CFGANALYZER_HH

/** @file */
