#include "FunctionManager.hh"

#include <iostream>
#include <algorithm>

namespace vy {

using namespace clang;

FunctionManager functionMgr;

CallInfo::CallInfo() { }

CallInfo::CallInfo(unsigned ID, bool isSimpleCall)
  : ID(ID), isSimpleCall(isSimpleCall)
{ }

FunctionInfo::FunctionInfo() { }

FunctionInfo::FunctionInfo(const FunctionDecl* decl, bool deleteSource) :
  name(decl->getNameAsString()), deleteSource(deleteSource) {

  for (auto p = decl->param_begin(); p != decl->param_end(); ++p)
    varSubs[(*p)->getNameAsString()] = (*p)->getNameAsString();

  auto body = cast<CompoundStmt>(decl->getBody());
  for (auto s = body->body_begin(); s != body->body_end(); ++s)
    findDeclarations(*s);
}

void FunctionInfo::findDeclarations(Stmt* stmt) {
  if (DeclStmt* declStmt = dyn_cast<DeclStmt>(stmt)) {
    for (auto d = declStmt->decl_begin(); d != declStmt->decl_end(); ++d)
      if (VarDecl* varDecl = dyn_cast<VarDecl>(*d))
        varSubs[varDecl->getNameAsString()] = varDecl->getNameAsString();
  }

  for (auto c : stmt->children())
    if (c != nullptr)
      findDeclarations(c);
}


FunctionManager::FunctionManager() { }

FunctionManager::container::iterator FunctionManager::find(const std::string& name) {
  return std::find_if(userFunctions_.begin(),
                      userFunctions_.end(),
                      [&](const FunctionInfo& fi) { return fi.name == name; });
}

void FunctionManager::addUserFunction(const clang::FunctionDecl* decl) {
  userFunctions_.emplace_back(decl);
}

bool FunctionManager::isUserDefined(const std::string& name) {
  return find(name) != userFunctions_.end();
}

void FunctionManager::setDeleteSource(const std::string& functionName, bool value) {
  auto found = find(functionName);
  if (found != userFunctions_.end()) {
    found->deleteSource = value;
  }
}

void FunctionManager::addCall(const clang::CallExpr* call, bool isSimpleCall) {
  auto found = find(call->getDirectCallee()->getNameAsString());
  if (found != userFunctions_.end()) {
    found->calls.emplace_back(call->getLocStart().getRawEncoding(), isSimpleCall);
  }
}

void FunctionManager::addCall(const clang::CallExpr* call, bool isSimpleCall,
                              const clang::SourceLocation& lineLoc) {
  auto found = find(call->getDirectCallee()->getNameAsString());
  if (found != userFunctions_.end()) {
    found->calls.emplace_back(call->getLocStart().getRawEncoding(), isSimpleCall);
    found->calls.back().lineStartLoc = lineLoc;
  }
}

bool FunctionManager::isSimpleCall(const clang::CallExpr* call) {
  auto found = find(call->getDirectCallee()->getNameAsString());
  if (found != userFunctions_.end()) {
    for (auto callIt : found->calls) {
      if (callIt.ID == call->getLocStart().getRawEncoding()) {
        return callIt.isSimpleCall;
      }
    }
  }

  return false;
}

const clang::SourceLocation& FunctionManager::getStmtLoc(clang::CallExpr* call) {
  auto function = find(call->getDirectCallee()->getNameAsString());
  auto callInfo = std::find_if(function->calls.begin(),
                               function->calls.end(),
                               [&](const CallInfo& ci) {
                                 return call->getLocStart().getRawEncoding() == ci.ID;
                               });
  return callInfo->lineStartLoc;
}

const std::map<std::string, std::string>& FunctionManager::getVarSubs(CallExpr* call) {
  auto function = find(call->getDirectCallee()->getNameAsString());
  return function->varSubs;
}


void FunctionManager::print() {
  for (auto i = userFunctions_.begin(); i != userFunctions_.end(); ++i) {
    std::cout << i->name << " " << i->deleteSource << std::endl;
  }
}

}
