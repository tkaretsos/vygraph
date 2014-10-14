#include "Translator.hh"

#include <iostream>

#include "clang/AST/Expr.h"
#include "clang/AST/Stmt.h"
#include "llvm/ADT/Optional.h"

#include "Utility/Functions.hh"

namespace vy {

using namespace clang;
using namespace std;

Translator::Translator(stringstream& sstream, ASTContext& context)
  : context(context), outs(sstream), indentLevel(0), pcCounter(0)
{ }

void
Translator::translateVarDecl(const clang::VarDecl* varDecl) {
  string str;
  if (varDecl->getType().getTypePtr()->isBooleanType()) {
    str = "bool ";
    str.append(varDecl->getNameAsString());
    str.append(" = ");
    if (varDecl->hasInit()) {
      string init(util::RangeToStr(varDecl->getInit()->getSourceRange(), context));
      str.append((init == "0") ? "false" : "true");
    } else {
      str.append("*");
    }
  } else {
    str = util::RangeToStr(varDecl->getSourceRange(), context);
    if (!varDecl->hasInit()) {
      str.append(" = *");
    }
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

  auto curBlock = cfg->getEntry();
  while (!curBlock.succ_empty()) {
    insertSequentialStmts(curBlock.begin(), curBlock.end());
    curBlock = **curBlock.succ_begin();
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

void
Translator::insertSequentialStmts(CFGBlock::const_iterator begin,
                                  CFGBlock::const_iterator end) {
  for (auto element = begin; element != end; ++element) {
    auto cfgStmt = element->getAs<CFGStmt>();
    if (cfgStmt.hasValue()) {
      auto stmt = cfgStmt->getStmt();
      switch (stmt->getStmtClass()) {

        default:
        {
          string stmtStr(util::RangeToStr(stmt->getSourceRange(), context));
          replaceAssignOp(stmtStr);
          insertLocationStr();
          stmtStr.append(";");
          outs << stmtStr << endl;
          break;
        }

        case Stmt::ReturnStmtClass:
        {
          continue;
          break;
        }

        case Stmt::DeclStmtClass:
        {
          auto declStmt = cast<DeclStmt>(stmt);
          if (auto varDecl = dyn_cast<VarDecl>(declStmt->getSingleDecl())) {
            translateVarDecl(varDecl);
          }
          continue;
          break;
        }
      }
    }
  }
}


void
Translator::insertLocationStr() {
  outs << indentStr << "pc" << pcCounter++ << " -> ";
  outs << "pc" << pcCounter << ": ";
}


} // namespace vy
