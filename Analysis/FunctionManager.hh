#ifndef VYGRAPH_FUNCTIONMANAGER_H
#define VYGRAPH_FUNCTIONMANAGER_H

#include <map>

#include "clang/AST/Decl.h"
#include "clang/Basic/SourceLocation.h"

namespace vy {

struct FunctionInfo {
  FunctionInfo();
  FunctionInfo(std::string, clang::SourceLocation, bool = true);

  std::string name;
  clang::SourceLocation location;
  bool deleteSource;
};

class FunctionManager {
  typedef std::map<std::string, FunctionInfo> container;

  container userFunctions_;
  FunctionManager(const FunctionManager&);
public:
  FunctionManager();
  void addUserFunction(const clang::FunctionDecl*);
  bool isUserDefined(const std::string&);
  void setDeleteSource(const std::string&, bool);
  void print();
};

extern FunctionManager functionMgr;

}

#endif // VYGRAPH_ANALYSISMANAGER_H
