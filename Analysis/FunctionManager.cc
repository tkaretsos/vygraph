#include "FunctionManager.hh"

namespace vy {

FunctionInfo::FunctionInfo() { }

FunctionInfo::FunctionInfo(std::string name, clang::SourceLocation location,
                           bool deleteDeclaration)
  : name(name), location(location), deleteDeclaration(deleteDeclaration)
{ }


FunctionManager::container FunctionManager::functions_;

void FunctionManager::add(const FunctionInfo& info) {
  FunctionManager::functions_.push_back(info);
}


void FunctionManager::add(const std::string& name,
                                 const clang::SourceLocation& location) {
  FunctionManager::functions_.emplace_back(name, location, false);
}

FunctionManager::iterator FunctionManager::begin() {
  return FunctionManager::functions_.begin();
}

FunctionManager::iterator FunctionManager::end() {
  return FunctionManager::functions_.end();
}

size_t FunctionManager::size() {
  return FunctionManager::functions_.size();
}

}
