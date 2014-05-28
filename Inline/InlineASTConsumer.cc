#include "InlineASTConsumer.hh"

namespace vy {

using namespace clang;

InlineASTConsumer::InlineASTConsumer(Rewriter& rewriter) : visitor(rewriter)
{ }

bool InlineASTConsumer::HandleTopLevelDecl(DeclGroupRef decls) {
  for (auto decl = decls.begin(); decl != decls.end(); ++decl)
    visitor.TraverseDecl(*decl);

  return true;
}

}
