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
  beginFunction(funcDecl);

  auto& curBlock = cfg->getEntry();
  while (!curBlock.succ_empty()) {
    auto terminatorCond = curBlock.getTerminatorCondition();
    if (terminatorCond != nullptr) {
      insertSequentialStmts(curBlock.begin(), curBlock.end() - 1);
      auto branchStartLoc = pcCounter;
      insertBranchCondTrue(terminatorCond);
      if ((*curBlock.succ_rbegin())->getBlockID() == getBranchExitID(curBlock)) {
        // if there is not an else part
        insertBranchTargetTrue(**curBlock.succ_begin());
        insertBranchCondFalse(terminatorCond, LocationPair(branchStartLoc, pcCounter));
      } else {
        // if there is an else part
        insertBranchTargetTrue(**curBlock.succ_begin(), getBranchEndLoc(curBlock));
        insertBranchCondFalse(terminatorCond, LocationPair(branchStartLoc, pcCounter));
        insertBranchTargetFalse(**curBlock.succ_rbegin());
      }
      curBlock = **curBlock.succ_begin();
    } else {
      insertSequentialStmts(curBlock.begin(), curBlock.end());
    }
    curBlock = **curBlock.succ_begin();
  }

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
Translator::beginFunction(const FunctionDecl* funcDecl) {
  analysis.reset(new AnalysisDeclContext(&analysisManager, funcDecl));
  cfg = analysis->getCFG();
  domTree.buildDominatorTree(*analysis);

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

Translator::LocationPair
Translator::getLocation() {
  LocationPair location;
  location.first = pcCounter;
  location.second = ++pcCounter;
  return location;
}

void Translator::insertStmt (const Stmt* stmt, const LocationPair* location) {
  switch (stmt->getStmtClass()) {
    default: {
      string stmtStr(util::RangeToStr(stmt->getSourceRange(), context));
      replaceAssignOp(stmtStr);
      stmtStr.append(";");
      outs << indentStr << ((location) ? *location : getLocation())
           << stmtStr << endl;
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
Translator::insertSequentialStmts(CFGBlock::const_iterator begin,
                                  CFGBlock::const_iterator end) {
  for (auto element = begin; element != end; ++element) {
    auto cfgStmt = element->getAs<CFGStmt>();
    if (cfgStmt.hasValue()) {
      insertStmt(cfgStmt->getStmt());
    }
  }
}

void
Translator::insertSequentialStmts(CFGBlock::const_iterator begin,
                                  CFGBlock::const_iterator end,
                                  unsigned int endLoc) {
  insertSequentialStmts(begin, end - 1);
  auto cfgStmt = (end - 1)->getAs<CFGStmt>();
  if (cfgStmt.hasValue()) {
    LocationPair loc(pcCounter++, endLoc);
    insertStmt(cfgStmt->getStmt(), &loc);
  }
}

void
Translator::insertBranchCondTrue(const Stmt* condition) {
  string stmtStr(util::RangeToStr(condition->getSourceRange(), context));
  stmtStr.append(";");
  outs << indentStr << getLocation() << stmtStr << endl;
}

void
Translator::insertBranchCondFalse(const Stmt* condition, const LocationPair& locs) {
  string stmtStr(util::RangeToStr(condition->getSourceRange(), context));
  stmtStr.insert(0, "!(");
  stmtStr.append(");");
  outs << indentStr << locs << stmtStr << endl;
}

void
Translator::insertBranchTargetTrue(const CFGBlock& curBlock) {
  insertSequentialStmts(curBlock.begin(), curBlock.end());
}

void
Translator::insertBranchTargetTrue(const CFGBlock& curBlock, unsigned int endLoc) {
  insertSequentialStmts(curBlock.begin(), curBlock.end(), endLoc);
}

void
Translator::insertBranchTargetFalse(const CFGBlock& curBlock) {
  insertSequentialStmts(curBlock.begin(), curBlock.end());
}

unsigned int
Translator::getBranchExitID(const CFGBlock& curBlock) const {
  auto target = *curBlock.succ_begin();
  auto targetSucc = *target->succ_begin();
  return targetSucc->getBlockID();
}

unsigned int
Translator::getBranchEndLoc(const CFGBlock& curBlock) const {
  return pcCounter + (*curBlock.succ_begin())->size()
                   + (*curBlock.succ_rbegin())->size();
}

ostream& operator<<(ostream& os, const Translator::LocationPair& loc) {
  os << "pc" << loc.first << " -> pc" << loc.second << ": ";
  return os;
}

} // namespace vy
