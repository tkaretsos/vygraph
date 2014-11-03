#include "Translator.hh"

#include <iostream>
#include <sstream>

#include "clang/AST/Expr.h"

#include "Utility/Functions.hh"

namespace vy {

using namespace clang;
using namespace std;

Translator::Translator(stringstream& sstream, ASTContext& context)
  : context(context), outs(sstream), indentLevel(0)
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
  analyzer.analyze(funcDecl);
  beginFunction(funcDecl);

  insertCFG(**analyzer.getCFG()->getEntry().succ_begin());

  endFunction();
  analyzer.finalize();
}

void
Translator::insertCFG(const CFGBlock& block) {

  if (block.getBlockID() == analyzer.getCFG()->getExit().getBlockID())
    return;

  writeStmts(block);
  if (block.succ_size() > 1) {
    insertCFG(**block.succ_begin());
    writeTerminatorFalse(block);
    if (hasElsePart(block))
      insertCFG(**block.succ_rbegin());
    if (auto postdom = analyzer.findFirstPostDominator(block)) {
      if (analyzer.getDomTree().dominates(&block, postdom))
        insertCFG(*postdom);
    }
  } else if (analyzer.getDomTree().dominates(&block, *block.succ_begin())) {
    insertCFG(**block.succ_begin());
  }
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
Translator::beginFunction(const FunctionDecl* funcDecl) {
  outs << indentStr << funcDecl->getNameAsString() << " {" << endl;
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
Translator::writeStmts(const CFGBlock& block) {
  for (auto elem = block.begin(); elem != block.end(); ++elem) {
    if (auto cfgStmt = elem->getAs<CFGStmt>()) {
      switch (cfgStmt->getStmt()->getStmtClass()) {
        default: {
          string stmtStr(util::RangeToStr(cfgStmt->getStmt()->getSourceRange(),
                                          context));
          replaceAssignOp(stmtStr);
          stmtStr.append(";");
          outs << indentStr << getLocString(block) << stmtStr << endl;
          break;
        }

        case Stmt::DeclStmtClass: {
          auto declStmt = cast<DeclStmt>(cfgStmt->getStmt());
          if (auto varDecl = dyn_cast<VarDecl>(declStmt->getSingleDecl()))
            translateVarDecl(varDecl);
          break;
        }

        case Stmt::ReturnStmtClass:
          break;
      }
    }
  }
}

void
Translator::writeTerminatorFalse(const CFGBlock& block) {
  string stmtStr(util::RangeToStr(block.getTerminatorCondition()->getSourceRange(),
                                  context));
  stmtStr.insert(0, "!(");
  stmtStr.append(");");

  string loc(analyzer.getLastLoc(block) + " -> ");
  loc.append(analyzer.getFirstAvailableLoc(**block.succ_rbegin()) + ": ");

  outs << indentStr << loc << stmtStr << endl;
}

bool
Translator::hasElsePart(const CFGBlock& block) const {
  if (auto postdom = analyzer.findFirstPostDominator(block))
    return postdom->getBlockID() != (*block.succ_rbegin())->getBlockID();
  return false;
}

string
Translator::getLocString(const CFGBlock& block) {
  string ret(analyzer.getCurrentLoc(block) + " -> ");
  if (analyzer.hasNextLoc(block)) {
    ret.append(analyzer.getNextLoc(block));
  } else {
    ret.append(analyzer.getFirstAvailableLoc(**block.succ_begin()));
  }
  ret.append(": ");
  return ret;
}

} // namespace vy
