#ifndef VYGRAPH_FUNCTIONMANAGER_H
#define VYGRAPH_FUNCTIONMANAGER_H

#include <vector>

#include "clang/AST/Decl.h"
#include "clang/Basic/SourceLocation.h"

namespace vy {

struct CallInfo {
  CallInfo();
  CallInfo(clang::SourceLocation&, bool);

  clang::SourceLocation location;
  bool isSimpleCall;
};

struct FunctionInfo {
  FunctionInfo();
  FunctionInfo(std::string, bool = true);

  std::vector<CallInfo> calls;
  std::string name;
  bool deleteSource;
};

class FunctionManager {
  typedef std::vector<FunctionInfo> container;

  container userFunctions_;
  FunctionManager(const FunctionManager&);

  container::iterator find(const std::string&);
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
