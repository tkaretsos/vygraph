#ifndef VYGRAPH_ANALYZER_HH
#define VYGRAPH_ANALYZER_HH

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"

#include "FunctionManager.hh"

namespace vy {

using clang::ast_matchers::MatchFinder;
using clang::tooling::ClangTool;

class Analyzer {
  ClangTool& tool;
  class FuntionLocator : public MatchFinder::MatchCallback {
    virtual void run(const MatchFinder::MatchResult&);
  };

public:
  Analyzer(ClangTool&);
  void analyze();
};

}

#endif // VYGRAPH_ANALYZER_HH
