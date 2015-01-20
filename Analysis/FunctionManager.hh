#ifndef VYGRAPH_FUNCTIONMANAGER_H
#define VYGRAPH_FUNCTIONMANAGER_H

#include <vector>
#include <map>

#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/SourceLocation.h"

namespace vy {

struct CallInfo {
  CallInfo();
  CallInfo(unsigned, bool);

  unsigned ID;
  bool isSimpleCall;
  clang::SourceLocation lineStartLoc;
};

struct FunctionInfo {
  FunctionInfo();
  FunctionInfo(const clang::FunctionDecl*, bool = true);

  std::vector<CallInfo> calls;
  std::string name;
  bool deleteSource;
  std::map<std::string, std::string> varSubs;

private:
  void findDeclarations(clang::Stmt*);
};

class FunctionManager {
  typedef std::vector<FunctionInfo> container;

  container userFunctions_;
  FunctionManager(const FunctionManager&);

  container::iterator find(const std::string&);

public:
  std::string fileContents;

  FunctionManager();
  void addUserFunction(const clang::FunctionDecl*);
  bool isUserDefined(const std::string&);
  void setDeleteSource(const std::string&, bool);
  void addCall(const clang::CallExpr*, bool);
  void addCall(const clang::CallExpr*, bool, const clang::SourceLocation&);
  bool isSimpleCall(const clang::CallExpr*);
  const clang::SourceLocation getInsertLoc(clang::CallExpr*);
  const clang::SourceLocation& getStmtLoc(clang::CallExpr*);
  const std::map<std::string, std::string>& getVarSubs(clang::CallExpr*);
  void print();
};

extern FunctionManager functionMgr;

}

#endif // VYGRAPH_ANALYSISMANAGER_H
