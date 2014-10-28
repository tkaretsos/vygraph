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
  beginFunction(funcDecl);

  insertSubCFG(**cfg->getEntry().succ_begin());

  endFunction();
}

void
Translator::insertSubCFG(const CFGBlock& block) {

  if (block.getBlockID() == cfg->getExit().getBlockID())
    return;
  if (analyzer.isInserted(block))
    return;
  else
    analyzer.setInsertedBlock(block);

  insertSequentialStmts(block);
  if (hasTerminator(block)) {
    insertSubCFG(**block.succ_begin());
    insertTerminatorFalse(block);
    insertSubCFG(**block.succ_rbegin());
    if (domTree.dominates(&block, &getPostDominator(block))) {
      insertSubCFG(getPostDominator(block));
    }
  } else if (domTree.dominates(&block, *block.succ_begin())) {
      insertSubCFG(**block.succ_begin());
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
  analyzer.analyze(cfg);
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

void
Translator::insertSequentialStmts(const CFGBlock& block) {
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
Translator::insertTerminatorFalse(const CFGBlock& block) {
  string stmtStr(util::RangeToStr(block.getTerminatorCondition()->getSourceRange(),
                                  context));
  stmtStr.insert(0, "!(");
  stmtStr.append(");");

  string loc(analyzer.getLastLoc(block) + " -> ");
  loc.append(analyzer.getFirstAvailableLoc(**block.succ_rbegin()) + ": ");

  outs << indentStr << loc << stmtStr << endl;
}

const CFGBlock&
Translator::getPostDominator(const CFGBlock& curBlock) const {
  auto branchStart = curBlock.succ_begin();
  auto block = (*branchStart)->succ_rbegin();
  while (domTree.dominates(*branchStart, *block))
    block = (*block)->succ_rbegin();
  return **block;
}

bool
Translator::hasElsePart(const CFGBlock& block) const {
  return (*block.succ_rbegin())->getBlockID() != getPostDominator(block).getBlockID();
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

bool
Translator::hasTerminator(const CFGBlock& block) const {
  return block.getTerminatorCondition() != nullptr;
}

} // namespace vy
