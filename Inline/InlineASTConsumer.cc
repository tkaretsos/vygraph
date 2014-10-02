#include "InlineASTConsumer.hh"

namespace vy {

using namespace clang;

InlineASTConsumer::InlineASTConsumer(Rewriter& rewriter, ASTContext& context)
  : inliner(rewriter, context)
{ }

bool
InlineASTConsumer::HandleTopLevelDecl(DeclGroupRef decls) {
  for (auto decl = decls.begin(); decl != decls.end(); ++decl)
    inliner.TraverseDecl(*decl);

  return true;
}

}
