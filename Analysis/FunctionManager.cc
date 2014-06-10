#include "FunctionManager.hh"

#include <iostream>

namespace vy {

FunctionManager functionMgr;

FunctionInfo::FunctionInfo() { }

FunctionInfo::FunctionInfo(std::string name, bool deleteSource)
  : name(name), deleteSource(deleteSource)
{ }

FunctionManager::FunctionManager() { }

void FunctionManager::addUserFunction(const clang::FunctionDecl* decl) {
  auto name = decl->getNameAsString();
  userFunctions_.emplace(name, FunctionInfo(name));
}

bool FunctionManager::isUserDefined(const std::string& name) {
  return userFunctions_.find(name) != userFunctions_.end();
}

void FunctionManager::setDeleteSource(const std::string& functionName, bool value) {
  if (isUserDefined(functionName)) {
    userFunctions_[functionName].deleteSource = value;
  }
}

void FunctionManager::print() {
  for (auto i = userFunctions_.begin(); i != userFunctions_.end(); ++i) {
    std::cout << i->second.name << " " << i->second.deleteSource << std::endl;
  }
}

}
