#ifndef VYGRAPH_ANALYSISMANAGER_H
#define VYGRAPH_ANALYSISMANAGER_H

#include <vector>

#include "clang/Basic/SourceLocation.h"

namespace vy {

struct FunctionInfo {
  FunctionInfo();
  FunctionInfo(std::string, clang::SourceLocation, bool = false);

  std::string name;
  clang::SourceLocation location;
  bool deleteDeclaration;
};

class FunctionManager {
  typedef std::vector<FunctionInfo> container;
  typedef container::const_iterator iterator;

  container functions_;
  FunctionManager(const FunctionManager&);
public:
  FunctionManager();
  void add(const FunctionInfo&);
  void add(const std::string&, const clang::SourceLocation&, bool = false);
  iterator begin();
  iterator end();
  size_t size();
};

extern FunctionManager functionMgr;

}

#endif // VYGRAPH_ANALYSISMANAGER_H
