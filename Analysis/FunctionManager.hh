#ifndef VYGRAPH_FUNCTIONMANAGER_H
#define VYGRAPH_FUNCTIONMANAGER_H

#include <vector>
#include <map>

#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/SourceLocation.h"

namespace vy {

struct CallInfo {
public:
  unsigned ID;
  bool isSimpleCall;
  clang::SourceLocation lineStartLoc;

  CallInfo() = default;
  CallInfo(unsigned, bool);

};

struct FunctionInfo {
public:
  std::vector<CallInfo> calls;
  std::string name;
  bool deleteSource;
  std::map<std::string, std::string> varSubs;

  FunctionInfo() = default;
  FunctionInfo(const clang::FunctionDecl*, bool = true);

private:
  void findDeclarations(clang::Stmt*);

};

class FunctionManager {
public:
  std::string fileContents;

  FunctionManager() = default;

  void addUserFunction(const clang::FunctionDecl*);
  bool isUserDefined(const std::string&);
  void setDeleteSource(const std::string&, bool);
  void addCall(const clang::CallExpr*, bool);
  void addCall(const clang::CallExpr*, bool, const clang::SourceLocation&);
  bool isSimpleCall(const clang::CallExpr*);
  const clang::SourceLocation getInsertLoc(clang::CallExpr*);
  const clang::SourceLocation& getStmtLoc(clang::CallExpr*);
  const std::map<std::string, std::string>& getVarSubs(clang::CallExpr*);

private:
  std::vector<FunctionInfo> userFunctions_;

  FunctionManager(const FunctionManager&);

  std::vector<FunctionInfo>::iterator find(const std::string&);

};

extern FunctionManager functionMgr;

}

#endif // VYGRAPH_ANALYSISMANAGER_H
