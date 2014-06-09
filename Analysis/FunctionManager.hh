#ifndef VYGRAPH_FUNCTIONMANAGER_H
#define VYGRAPH_FUNCTIONMANAGER_H

#include <vector>

#include "clang/Basic/SourceLocation.h"

namespace vy {

struct FunctionInfo {
  FunctionInfo();
  FunctionInfo(std::string, clang::SourceLocation, bool = true);

  std::string name;
  clang::SourceLocation location;
  bool deleteDeclaration;
};

class FunctionManager {
  typedef std::vector<FunctionInfo> container;
  typedef container::const_iterator iterator;

  container userFunctions_;
  FunctionManager(const FunctionManager&);
public:
  FunctionManager();
  void addUserFunction(const FunctionInfo&);
  void addUserFunction(const std::string&, const clang::SourceLocation&,
                       bool = true);
};

extern FunctionManager functionMgr;

}

#endif // VYGRAPH_ANALYSISMANAGER_H
