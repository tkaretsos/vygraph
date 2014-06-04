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

  static container functions_;
  FunctionManager();
  FunctionManager(const FunctionManager&);
public:
  static void add(const FunctionInfo&);
  static void add(const std::string&, const clang::SourceLocation&);
  static iterator begin();
  static iterator end();
  static size_t size();
};

}

#endif // VYGRAPH_ANALYSISMANAGER_H
