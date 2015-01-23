#include "FunctionManager.hh"

#include <iostream>
#include <algorithm>

namespace vy {

using namespace clang;

FunctionManager functionMgr;

CallInfo::CallInfo(unsigned ID, bool isSimpleCall)
  : ID(ID), isSimpleCall(isSimpleCall)
{ }

FunctionInfo::FunctionInfo(const FunctionDecl* decl, bool deleteSource) :
  name(decl->getNameAsString()), deleteSource(deleteSource) {

  for (auto param = decl->param_begin(); param != decl->param_end(); ++param)
    varSubs[(*param)->getNameAsString()] = (*param)->getNameAsString();

  auto body = cast<CompoundStmt>(decl->getBody());
  for (auto stmt = body->body_begin(); stmt != body->body_end(); ++stmt)
    findDeclarations(*stmt);
}

void
FunctionInfo::findDeclarations(Stmt* stmt) {
  if (DeclStmt* declStmt = dyn_cast<DeclStmt>(stmt)) {
    for (auto decl = declStmt->decl_begin(); decl != declStmt->decl_end(); ++decl)
      if (VarDecl* varDecl = dyn_cast<VarDecl>(*decl))
        varSubs[varDecl->getNameAsString()] = varDecl->getNameAsString();
  }

  for (auto c : stmt->children())
    if (c != nullptr)
      findDeclarations(c);
}

std::vector<FunctionInfo>::iterator
FunctionManager::find(const std::string& name) {
  auto pred = [&](const FunctionInfo& fi) { return fi.name == name; };
  return std::find_if(userFunctions_.begin(), userFunctions_.end(), pred);
}

void
FunctionManager::addUserFunction(const clang::FunctionDecl* decl) {
  userFunctions_.emplace_back(decl);
}

bool
FunctionManager::isUserDefined(const std::string& name) {
  return find(name) != userFunctions_.end();
}

void
FunctionManager::setDeleteSource(const std::string& functionName, bool value) {
  auto found = find(functionName);
  if (found != userFunctions_.end())
    found->deleteSource = value;
}

void
FunctionManager::addCall(const clang::CallExpr* call, bool isSimpleCall) {
  auto found = find(call->getDirectCallee()->getNameAsString());
  if (found != userFunctions_.end())
    found->calls.emplace_back(call->getLocStart().getRawEncoding(), isSimpleCall);
}

void
FunctionManager::addCall(const clang::CallExpr* call, bool isSimpleCall,
                         const clang::SourceLocation& lineLoc) {
  auto found = find(call->getDirectCallee()->getNameAsString());
  if (found != userFunctions_.end()) {
    found->calls.emplace_back(call->getLocStart().getRawEncoding(), isSimpleCall);
    found->calls.back().lineStartLoc = lineLoc;
  }
}

bool
FunctionManager::isSimpleCall(const clang::CallExpr* call) {
  auto found = find(call->getDirectCallee()->getNameAsString());
  if (found != userFunctions_.end()) {
    for (auto callIt : found->calls) {
      if (callIt.ID == call->getLocStart().getRawEncoding())
        return callIt.isSimpleCall;
    }
  }

  return false;
}

const clang::SourceLocation&
FunctionManager::getStmtLoc(clang::CallExpr* call) {
  auto function = find(call->getDirectCallee()->getNameAsString());
  auto pred = [&](const CallInfo& ci) {
    return call->getLocStart().getRawEncoding() == ci.ID;
  };
  auto callInfo = std::find_if(function->calls.begin(), function->calls.end(), pred);
  return callInfo->lineStartLoc;
}

const std::map<std::string, std::string>&
FunctionManager::getVarSubs(CallExpr* call) {
  auto function = find(call->getDirectCallee()->getNameAsString());
  return function->varSubs;
}

const SourceLocation
FunctionManager::getInsertLoc(CallExpr* call) {
  return (isSimpleCall(call)) ? call->getLocStart() : getStmtLoc(call);
}

} // namespace vy
