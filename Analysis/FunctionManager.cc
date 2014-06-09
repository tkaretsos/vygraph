#include "FunctionManager.hh"

namespace vy {

FunctionManager functionMgr;

FunctionInfo::FunctionInfo() { }

FunctionInfo::FunctionInfo(std::string name, clang::SourceLocation location,
                           bool deleteDeclaration)
  : name(name), location(location), deleteDeclaration(deleteDeclaration)
{ }

FunctionManager::FunctionManager() { }

void FunctionManager::addUserFunction(const FunctionInfo& info) {
  userFunctions_.push_back(info);
}

void FunctionManager::addUserFunction(const std::string& name,
                                      const clang::SourceLocation& location,
                                      bool deleteDeclaration) {
  userFunctions_.emplace_back(name, location, deleteDeclaration);
}

}
