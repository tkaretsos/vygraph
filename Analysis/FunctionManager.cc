#include "FunctionManager.hh"

namespace vy {

FunctionManager functionMgr;

FunctionInfo::FunctionInfo() { }

FunctionInfo::FunctionInfo(std::string name, clang::SourceLocation location,
                           bool deleteDeclaration)
  : name(name), location(location), deleteDeclaration(deleteDeclaration)
{ }

FunctionManager::FunctionManager() { }

void FunctionManager::add(const FunctionInfo& info) {
  functions_.push_back(info);
}

void FunctionManager::add(const std::string& name,
                          const clang::SourceLocation& location,
                          bool deleteDeclaration) {
  functions_.emplace_back(name, location, deleteDeclaration);
}

FunctionManager::iterator FunctionManager::begin() {
  return functions_.begin();
}

FunctionManager::iterator FunctionManager::end() {
  return functions_.end();
}

size_t FunctionManager::size() {
  return functions_.size();
}

}
