#include "TranslateASTConsumer.hh"

#include "clang/Analysis/CFG.h"
#include "clang/Lex/Lexer.h"

#include <iostream>
using namespace std;

#include "../Analysis/FunctionManager.hh"
#include "../Utility/Functions.hh"

namespace vy {

using namespace clang;

TranslateASTConsumer::TranslateASTConsumer(ASTContext& context,
                                           stringstream& outstream)
  : context(context), outs(outstream), indentLevel(0)
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

void
TranslateASTConsumer::insertVarDecl(VarDecl* varDecl) {
  string str;
  if (varDecl->getType().getTypePtr()->isBooleanType()) {
    str = "bool ";
    str.append(varDecl->getNameAsString());
    if (varDecl->hasInit()) {
      str.append(" = ");
      str.append(util::RangeToStr(varDecl->getInit()->getSourceRange(), context));
    }
  } else {
    str = util::RangeToStr(varDecl->getSourceRange(), context);
  }

  auto found = str.find('=');
  if (found != string::npos)
    str.insert(found, ":");
  str.append(";");

  outs << str << endl;
}

void
TranslateASTConsumer::indent() {
  indentStr.assign(2 * ++indentLevel, ' ');
}

void
TranslateASTConsumer::unindent() {
  indentStr.assign(2 * --indentLevel, ' ');
}

} // namespace vy
