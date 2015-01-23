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
    str.assign("bool " + varDecl->getNameAsString() + " = ");
    if (varDecl->hasInit()) {
      string init(util::range_to_str(varDecl->getInit()->getSourceRange(), context));
      if (init == "0")
        str.append("false");
      else if (init == "1")
        str.append("true");
      else
        str.append(init);
    } else {
      str.append("*");
    }
  } else {
    str = util::range_to_str(varDecl->getSourceRange(), context);
    if (!varDecl->hasInit())
      str.append(" = *");
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
    for (auto succ = block.succ_rbegin() + 1; succ != block.succ_rend(); ++succ)
      writeCFG(**succ);

    writeTerminatorFalse(block);

    if (!analyzer.getPostDomTree().dominates(*block.succ_rbegin(), &block))
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
  outs << endl << indentStr << funcDecl->getNameAsString() << " {" << endl;
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
Translator::replaceEqualsOp(string& expr) const {
  auto found = expr.find("==");
  while (found != string::npos) {
    expr.replace(found, 2, "=");
    found = expr.find("==");
  }
}

void
Translator::writeStatements(const CFGBlock& block) {
  if (block.empty())
    return;

  for (auto elem = block.begin(); elem != block.end() - 1; ++elem) {
    if (auto cfgStmt = elem->getAs<CFGStmt>())
      writeStmt(block, cfgStmt->getStmt());
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
    default:
      writeDefaultStmt(block, stmt);
      break;

    case Stmt::BinaryOperatorClass:
      writeBinaryOp(block, cast<BinaryOperator>(stmt));
      break;

    case Stmt::CallExprClass:
      writeCustomFunctionCall(block, cast<CallExpr>(stmt));
      break;

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
Translator::writeDefaultStmt(const CFGBlock& block, const Stmt* stmt) {
  string stmtStr(util::range_to_str(stmt->getSourceRange(), context));
  replaceAssignOp(stmtStr);
  stmtStr.append(";");
  outs << indentStr << analyzer.getLocString(block) << stmtStr << endl;
}

void
Translator::writeBinaryOp(const CFGBlock& block, const BinaryOperator* binOp) {
  if (binOp->getLHS()->getType().getTypePtr()->isBooleanType()) {
    string rhsStr(util::range_to_str(binOp->getRHS()->getSourceRange(), context));
    if (rhsStr == "0" || rhsStr == "1") {
      string str(util::range_to_str(binOp->getLHS()->getSourceRange(), context));
      str.append(" := ");
      str.append((rhsStr == "0") ? "false" : "true");
      str.append(";");
      outs << indentStr << analyzer.getLocString(block) << str << endl;
      return;
    }
  }

  writeDefaultStmt(block, binOp);
}

void
Translator::writeTerminatorFalse(const CFGBlock& block) {
  string stmtStr("assume(!(");
  stmtStr.append(util::range_to_str(block.getTerminatorCondition()->getSourceRange(),
                                    context));
  stmtStr.append("));");

  auto found = stmtStr.find("non_deterministic");
  if (found != string::npos) {
    outs << indentStr << analyzer.getTerminatorFalseLoc(block) << "assume(true);" << endl;
    return;
  }

  replaceEqualsOp(stmtStr);

  outs << indentStr << analyzer.getTerminatorFalseLoc(block) << stmtStr << endl;
}

void
Translator::writeAssert(const CFGBlock& block, const Expr* expr) {
  string exprStr(util::range_to_str(expr->getSourceRange(), context));
  string str("assume(!(" + exprStr + "));");
  replaceEqualsOp(str);
  outs << indentStr << analyzer.getLocString(block, true) << str << endl;

  writeAssume(block, expr);
}

void
Translator::writeAssume(const CFGBlock& block, const Stmt* condition) {
  auto condStr = util::range_to_str(condition->getSourceRange(), context);
  auto found = condStr.find("non_deterministic");
  if (found != string::npos) {
    outs << indentStr << analyzer.getLocString(block) << "assume(true);" << endl;
    return;
  }

  string str("assume(");
  str.append(util::range_to_str(condition->getSourceRange(), context));
  str.append(");");
  replaceEqualsOp(str);
  outs << indentStr << analyzer.getLocString(block) << str << endl;
}

void
Translator::writeCustomFunctionCall(const CFGBlock& block, const CallExpr* call) {
  if (call->getDirectCallee()->getNameAsString() == "vy_atomic_begin") {
    beginAtomic();
    return;
  }

  if (call->getDirectCallee()->getNameAsString() == "vy_atomic_end") {
    endAtomic();
    return;
  }

  if (call->getDirectCallee()->getNameAsString() == "vy_assert") {
    writeAssert(block, call->getArg(0));
    return;
  }

  if (call->getDirectCallee()->getNameAsString() == "vy_assume") {
    writeAssume(block, call->getArg(0));
    return;
  }

  writeDefaultStmt(block, call);
}

} // namespace vy
