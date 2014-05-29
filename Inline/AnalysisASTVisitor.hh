#ifndef VYGRAPH_ANALYSISASTVISITOR_HH
#define VYGRAPH_ANALYSISASTVISITOR_HH

#include "clang/AST/RecursiveASTVisitor.h"

#include "../FunctionManager.hh"

namespace vy {

class AnalysisASTVisitor : public clang::RecursiveASTVisitor<AnalysisASTVisitor> {
  FunctionManager& funMgr;
public:
  AnalysisASTVisitor(FunctionManager&);
};

} // namespace vy

#endif // VYGRAPH_ANALYSISASTVISITOR_HH
