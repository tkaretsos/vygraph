#include "InlineASTConsumer.hh"

namespace vy {

using namespace clang;

InlineASTConsumer::InlineASTConsumer(Rewriter& rewriter) : inliner(rewriter)
{ }

bool InlineASTConsumer::HandleTopLevelDecl(DeclGroupRef decls) {
  for (auto decl = decls.begin(); decl != decls.end(); ++decl)
    inliner.TraverseDecl(*decl);

  return true;
}

}
