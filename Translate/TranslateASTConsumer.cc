#include "TranslateASTConsumer.hh"

#include "Analysis/FunctionManager.hh"

namespace vy {

using namespace clang;

TranslateASTConsumer::TranslateASTConsumer(ASTContext& context,
                                           std::stringstream& outstream)
  : translator(outstream, context)
{ }

bool
TranslateASTConsumer::HandleTopLevelDecl(DeclGroupRef decls) {
  for (auto decl = decls.begin(); decl != decls.end(); ++decl) {
    if (auto varDecl = dyn_cast<VarDecl>(*decl))
      translator.translateVarDecl(varDecl);

    if (auto funcDecl = dyn_cast<FunctionDecl>(*decl)) {
      if (functionMgr.isUserDefined(funcDecl->getNameAsString()) || funcDecl->isMain())
        translator.translateFunction(funcDecl);
    }
  }

  return true;
}

} // namespace vy

/** @file */
