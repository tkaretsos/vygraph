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
cfg->dump(context.getLangOpts(), true);
  analyzer.analyze(cfg);
  insertSubCFG(**cfg->getEntry().succ_begin());

  endFunction();
}

void
Translator::insertSubCFG(const CFGBlock& block) {
  insertSequentialStmts(block);
  if (hasTerminator(block)) {
    insertSubCFG(**block.succ_begin());
    insertBranchCondFalse(block);
    if (hasElsePart(block))
      insertSubCFG(**block.succ_rbegin());
    if (domTree.dominates(&block, &getBranchExitBlock(block))) {
      insertSubCFG(getBranchExitBlock(block));
    }
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
  analysis.reset(new AnalysisDeclContext(&analysisManager, funcDecl));
  cfg = analysis->getCFG();
  domTree.buildDominatorTree(*analysis);

  outs << indentStr << funcDecl->getNameAsString() << " {" << endl;
  indent();
}

void
Translator::endFunction() {
  domTree.releaseMemory();
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

void
Translator::insertStmt(const Stmt* stmt, const LocationPair*) {
  switch (stmt->getStmtClass()) {
    default: {
      string stmtStr(util::RangeToStr(stmt->getSourceRange(), context));
      replaceAssignOp(stmtStr);
      stmtStr.append(";");
      outs << indentStr << stmtStr << endl;
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
Translator::insertSequentialStmts(const CFGBlock& block) {
  for (auto elem = block.begin(); elem != block.end(); ++elem) {
    if (auto cfgStmt = elem->getAs<CFGStmt>()) {
      switch (cfgStmt->getStmt()->getStmtClass()) {
        default: {
          string stmtStr(util::RangeToStr(cfgStmt->getStmt()->getSourceRange(), context));
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
                                  const unsigned int* startLoc,
                                  const unsigned int* endLoc) {

  if (startLoc == nullptr && endLoc == nullptr) {
    insertSequentialStmts(begin, end);
  } else if (startLoc == nullptr) {
    insertSequentialStmts(begin, end - 1);
    auto cfgStmt = (end - 1)->getAs<CFGStmt>();
    if (cfgStmt.hasValue()) {
      LocationPair loc(pcCounter++, *endLoc);
      insertStmt(cfgStmt->getStmt(), &loc);
    }
  } else if (endLoc == nullptr) {
    auto cfgStmt = begin->getAs<CFGStmt>();
    if (cfgStmt.hasValue()) {
      LocationPair loc(*startLoc, pcCounter); // no pcCounter++
      insertStmt(cfgStmt->getStmt(), &loc);
    }
  } else {
    if ((end - begin) == 1) {
      auto cfgStmt = begin->getAs<CFGStmt>();
      if (cfgStmt.hasValue()) {
        LocationPair loc(*startLoc, *endLoc);
        insertStmt(cfgStmt->getStmt(), &loc);
      }
    } else {
      auto cfgStmt = begin->getAs<CFGStmt>();
      if (cfgStmt.hasValue()) {
        LocationPair loc(*startLoc, pcCounter++);
        insertStmt(cfgStmt->getStmt(), &loc);
      }
      insertSequentialStmts(begin + 1, end - 1);
      cfgStmt = (end - 1)->getAs<CFGStmt>();
      if (cfgStmt.hasValue()) {
        LocationPair loc(pcCounter++, *endLoc);
        insertStmt(cfgStmt->getStmt(), &loc);
      }
    }
  }
}

void
Translator::insertBranchCondTrue(const Stmt* condition) {
  string stmtStr(util::RangeToStr(condition->getSourceRange(), context));
  stmtStr.append(";");
  outs << indentStr << getLocation() << stmtStr << endl;
}

void
Translator::insertBranchCondFalse(const CFGBlock& block) {
  string stmtStr(util::RangeToStr(block.getTerminatorCondition()->getSourceRange(), context));
  stmtStr.insert(0, "!(");
  stmtStr.append(");");

  string loc(analyzer.getLastLoc(block) + " -> " + analyzer.getFirstLoc(**block.succ_rbegin()) + ": ");

  outs << indentStr << loc << stmtStr << endl;
}

const CFGBlock&
Translator::getBranchExitBlock(const CFGBlock& curBlock) const {
  auto branchStart = curBlock.succ_begin();
  auto block = (*branchStart)->succ_begin();
  while (domTree.dominates(*branchStart, *block))
    block = (*block)->succ_begin();
  return **block;
}

bool
Translator::hasElsePart(const CFGBlock& block) const {
  return (*block.succ_rbegin())->getBlockID() != getBranchExitBlock(block).getBlockID();
}

string
Translator::getLocString(const CFGBlock& block) {
  string ret(analyzer.getCurrentLoc(block) + " -> ");
  if (analyzer.hasNextLoc(block)) {
    ret.append(analyzer.getNextLoc(block));
  } else {
    ret.append(analyzer.getFirstLoc(**block.succ_begin()));
  }
  ret.append(": ");
  return ret;
}

bool
Translator::hasTerminator(const CFGBlock& block) const {
  return block.getTerminatorCondition() != nullptr;
}

ostream& operator<<(ostream& os, const Translator::LocationPair& loc) {
  os << "pc" << loc.first << " -> pc" << loc.second << ": ";
  return os;
}

} // namespace vy
