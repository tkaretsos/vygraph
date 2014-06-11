#include "FunctionManager.hh"

#include <iostream>
#include <algorithm>

namespace vy {

FunctionManager functionMgr;

CallInfo::CallInfo() { }

CallInfo::CallInfo(clang::SourceLocation& location, bool isSimpleCall)
  : location(location), isSimpleCall(isSimpleCall)
{ }

FunctionInfo::FunctionInfo() { }

FunctionInfo::FunctionInfo(std::string name, bool deleteSource)
  : name(name), deleteSource(deleteSource)
{ }

FunctionManager::FunctionManager() { }

FunctionManager::container::iterator FunctionManager::find(const std::string& name) {
  return std::find_if(userFunctions_.begin(),
                      userFunctions_.end(),
                      [&](const FunctionInfo& fi) { return fi.name == name; });
}

void FunctionManager::addUserFunction(const clang::FunctionDecl* decl) {
  auto name = decl->getNameAsString();
  userFunctions_.emplace_back(name);
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

void FunctionManager::print() {
  for (auto i = userFunctions_.begin(); i != userFunctions_.end(); ++i) {
    std::cout << i->name << " " << i->deleteSource << std::endl;
  }
}

}
