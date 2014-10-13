#include "Translator.hh"

#include <iostream>

#include "clang/Analysis/CFG.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Stmt.h"
#include "llvm/ADT/Optional.h"

#include "Utility/Functions.hh"

namespace vy {

using namespace clang;
using namespace std;

Translator::Translator(stringstream& sstream, ASTContext& context)
  : context(context), outs(sstream), indentLevel(0)
{ }

void
Translator::translateVarDecl(clang::VarDecl* varDecl) {
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

  replaceAssignOp(str);
  str.append(";");

  outs << indentStr << str << endl;
}

void
Translator::translateFunction(clang::FunctionDecl* funcDecl) {
  beginFunction(funcDecl->getNameAsString());
  CFG* cfg = CFG::buildCFG(funcDecl, funcDecl->getBody(),
                           &context, CFG::BuildOptions());

  auto cfgBegin = cfg->begin() + 1;
  auto cfgEnd = cfg->end() - 1;
  for (auto block = cfgBegin; block != cfgEnd; ++block) {
    for (auto element = (*block)->begin(); element != (*block)->end(); ++element) {
      auto cfgStmt = element->getAs<CFGStmt>();
      if (cfgStmt.hasValue()) {
        auto stmt = cfgStmt->getStmt();
        if (isa<ReturnStmt>(stmt))
          continue;
        string stmtStr(util::RangeToStr(stmt->getSourceRange(), context));
        replaceAssignOp(stmtStr);
        if (isa<Expr>(stmt))
          stmtStr.append(";");
        outs << indentStr << stmtStr << endl;
      }
    }
  }

  delete cfg;
  endFunction();
}

void
Translator::indent() {
  indentStr.assign(2 * ++indentLevel, ' ');
}

void
Translator::unindent() {
  indentStr.assign(2 * --indentLevel, ' ');
}

void
Translator::beginFunction(const std::string& functionName) {
  outs << indentStr << functionName << " {" << endl;
  indent();
}

void
Translator::endFunction() {
  unindent();
  outs << indentStr << "}" << endl;
}

void
Translator::replaceAssignOp(std::string& expr) const {
  auto found = expr.find('=');
  if (found != string::npos)
    expr.insert(found, ":");
}

} // namespace vy
