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

  writeCFG(**analyzer.getCFG()->getEntry().succ_begin());

  endFunction();
  analyzer.finalize();
}

void
Translator::writeCFG(const CFGBlock& block) {

  if (block.getBlockID() == analyzer.getCFG()->getExit().getBlockID())
    return;

  writeStatements(block);
  if (block.succ_size() > 1) {
    writeCFG(**block.succ_begin());
    writeTerminatorFalse(block);
    if (hasElsePart(block))
      writeCFG(**block.succ_rbegin());
    if (auto postdom = analyzer.findFirstPostDominator(block)) {
      if (analyzer.getDomTree().dominates(&block, postdom))
        writeCFG(*postdom);
    }
  } else if (analyzer.getDomTree().dominates(&block, *block.succ_begin())) {
    writeCFG(**block.succ_begin());
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
Translator::beginAtomic() {
  outs << indentStr << "atomic {" << endl;
  indent();
}

void
Translator::endAtomic() {
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
Translator::writeStatements(const CFGBlock& block) {
  if (block.empty())
    return;

  for (auto elem = block.begin(); elem != block.end() - 1; ++elem) {
    if (auto cfgStmt = elem->getAs<CFGStmt>()) {
      writeStmt(block, cfgStmt->getStmt());
    }
  }

  if (auto cfgStmt = (block.rbegin())->getAs<CFGStmt>()) {
    if (block.succ_size() > 1)
      writeAssume(block, cfgStmt->getStmt());
    else
      writeStmt(block, cfgStmt->getStmt());
  }
}

void
Translator::writeStmt(const CFGBlock& block, const Stmt* stmt) {
  switch (stmt->getStmtClass()) {
    default: {
      string stmtStr(util::RangeToStr(stmt->getSourceRange(), context));
      replaceAssignOp(stmtStr);
      stmtStr.append(";");
      outs << indentStr << getLocString(block) << stmtStr << endl;
      break;
    }

    case Stmt::CallExprClass: {
      auto call = cast<CallExpr>(stmt);
      if (call->getDirectCallee()->getNameAsString() == "vy_atomic_begin")
        beginAtomic();

      if (call->getDirectCallee()->getNameAsString() == "vy_atomic_end")
        endAtomic();

      if (call->getDirectCallee()->getNameAsString() == "vy_assert")
        writeAssert(block, call->getArg(0));

      if (call->getDirectCallee()->getNameAsString() == "vy_assume")
        writeAssume(block, call->getArg(0));

      break;
    }

    case Stmt::DeclStmtClass: {
      auto declStmt = cast<DeclStmt>(stmt);
      if (auto varDecl = dyn_cast<VarDecl>(declStmt->getSingleDecl()))
        translateVarDecl(varDecl);
      break;
    }

    case Stmt::ReturnStmtClass:
      break;
  }
}

void
Translator::writeTerminatorFalse(const CFGBlock& block) {
  string stmtStr("assume(!(");
  stmtStr.append(util::RangeToStr(block.getTerminatorCondition()->getSourceRange(),
                                  context));
  stmtStr.append("));");

  string loc(analyzer.getLastLoc(block) + " -> ");
  loc.append(analyzer.getFirstAvailableLoc(**block.succ_rbegin()) + ": ");

  outs << indentStr << loc << stmtStr << endl;
}

void
Translator::writeAssert(const CFGBlock& block, const Expr* expr) {
  string exprStr(util::RangeToStr(expr->getSourceRange(), context));
  string str("assume(!(" + exprStr + "));");
  outs << indentStr << getLocString(block, true) << str << endl;

  writeAssume(block, expr);
}

void
Translator::writeAssume(const CFGBlock& block, const Stmt* condition) {
  string str("assume(");
  str.append(util::RangeToStr(condition->getSourceRange(), context));
  str.append(");");
  outs << indentStr << getLocString(block) << str << endl;
}

bool
Translator::hasElsePart(const CFGBlock& block) const {
  if (auto postdom = analyzer.findFirstPostDominator(block))
    return postdom->getBlockID() != (*block.succ_rbegin())->getBlockID();
  return false;
}

string
Translator::getLocString(const CFGBlock& block, bool toErr) {
  string ret(analyzer.getCurrentLoc(block) + " -> ");
  if (toErr) {
    ret.append("err");
  } else {
    if (analyzer.hasNextLoc(block)) {
      ret.append(analyzer.getNextLoc(block));
    } else {
      ret.append(analyzer.getFirstAvailableLoc(**block.succ_begin()));
    }
  }
  ret.append(": ");
  return ret;
}

} // namespace vy
