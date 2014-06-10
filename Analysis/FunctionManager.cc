#include "FunctionManager.hh"

#include <iostream>

namespace vy {

FunctionManager functionMgr;

FunctionInfo::FunctionInfo() { }

FunctionInfo::FunctionInfo(std::string name, clang::SourceLocation location,
                           bool deleteSource)
  : name(name), location(location), deleteSource(deleteSource)
{ }

FunctionManager::FunctionManager() { }

void FunctionManager::addUserFunction(const clang::FunctionDecl* decl) {
  auto name = decl->getNameAsString();
  userFunctions_.emplace(name, FunctionInfo(name, decl->getLocStart()));
}

void FunctionManager::setDeleteSource(std::string functionName, bool value) {
  if (userFunctions_.find(functionName) != userFunctions_.end()) {
    userFunctions_[functionName].deleteSource = value;
  }
}

void FunctionManager::print() {
  for (auto i = userFunctions_.begin(); i != userFunctions_.end(); ++i) {
    std::cout << i->second.name << " " << i->second.deleteSource << std::endl;
  }
}

}
