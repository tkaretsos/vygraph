#ifndef VYGRAPH_INLINEASTCONSUMER_HH
#define VYGRAPH_INLINEASTCONSUMER_HH

#include "clang/AST/ASTConsumer.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include "AnalysisASTVisitor.hh"
#include "InlineASTVisitor.hh"

namespace vy {

class InlineASTConsumer : public clang::ASTConsumer {
  AnalysisASTVisitor analyzer;
  InlineASTVisitor inliner;
public:
  InlineASTConsumer(clang::Rewriter&);
  bool HandleTopLevelDecl(clang::DeclGroupRef) override;
};

} // namespace vy

#endif // VYGRAPH_INLINEASTCONSUMER_HH
