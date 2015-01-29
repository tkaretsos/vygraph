#include "InlineASTConsumer.hh"

namespace vy {

using namespace clang;

InlineASTConsumer::InlineASTConsumer(Rewriter& rewriter, ASTContext& context)
  : inlineVisitor(rewriter, context)
{ }

bool
InlineASTConsumer::HandleTopLevelDecl(DeclGroupRef decls) {
  for (auto decl = decls.begin(); decl != decls.end(); ++decl)
    inlineVisitor.TraverseDecl(*decl);

  return true;
}

}

/** @file */
