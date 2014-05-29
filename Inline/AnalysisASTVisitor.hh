#ifndef VYGRAPH_ANALYSISASTVISITOR_HH
#define VYGRAPH_ANALYSISASTVISITOR_HH

#include "clang/AST/RecursiveASTVisitor.h"

namespace vy {

class AnalysisASTVisitor : public clang::RecursiveASTVisitor<AnalysisASTVisitor> {
public:
  AnalysisASTVisitor();
};

} // namespace vy

#endif // VYGRAPH_ANALYSISASTVISITOR_HH
