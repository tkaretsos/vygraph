#include "TranslateASTConsumer.hh"

#include "clang/Analysis/CFG.h"
#include "clang/Lex/Lexer.h"

#include <iostream>
using namespace std;

#include "../Analysis/FunctionManager.hh"

namespace vy {

using namespace clang;

TranslateASTConsumer::TranslateASTConsumer(ASTContext& context,
                                           stringstream& outstream)
  : context(context), outs(outstream)
{ }

bool
TranslateASTConsumer::HandleTopLevelDecl(DeclGroupRef decls) {
  for (auto decl = decls.begin(); decl != decls.end(); ++decl) {
    if (auto varDecl = dyn_cast<VarDecl>(*decl))
      insertVarDecl(varDecl);
//     if (auto funcDecl = dyn_cast<FunctionDecl>(*decl)) {
//       if (functionMgr.isUserDefined(funcDecl->getNameAsString()) ||
//           funcDecl->isMain()) {
//         cout << funcDecl->getNameAsString() << endl;
//         CFG* cfg = CFG::buildCFG(funcDecl, funcDecl->getBody(),
//                                  &context, CFG::BuildOptions());
//         cfg->dump(LangOptions(), true);
//         delete cfg;
//       }
//     }
  }
  return true;
}

string
TranslateASTConsumer::getAsString(const SourceRange& range) {
  auto& SM = context.getSourceManager();
  auto charRange = CharSourceRange::getTokenRange(range);
  auto beginLoc = SM.getSpellingLoc(charRange.getBegin());
  auto endLoc = SM.getSpellingLoc(charRange.getEnd());
  auto size = SM.getDecomposedLoc(endLoc).second -
              SM.getDecomposedLoc(beginLoc).second;
  size += Lexer::MeasureTokenLength(endLoc, SM, context.getLangOpts());

  string str(SM.getCharacterData(beginLoc), size);
  str.append(";");

  return str;
}

void
TranslateASTConsumer::insertVarDecl(VarDecl* varDecl) {
  string str;
  if (varDecl->getType().getTypePtr()->isBooleanType()) {
    str = "bool ";
    str.append(varDecl->getNameAsString());
    if (varDecl->hasInit()) {
      str.append(" = ");
      str.append(getAsString(varDecl->getInit()->getSourceRange()));
    } else {
      str.append(";");
    }
  } else {
    str = getAsString(varDecl->getSourceRange());
  }

  auto found = str.find('=');
  if (found != string::npos)
    str.insert(found, ":");

  outs << str << endl;
}



} // namespace vy
