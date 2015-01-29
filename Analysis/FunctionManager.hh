#ifndef VYGRAPH_FUNCTIONMANAGER_H
#define VYGRAPH_FUNCTIONMANAGER_H

#include <vector>
#include <map>

#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/SourceLocation.h"

namespace vy {

/**
 * @brief Stores information about a call to a user defined function.
 *
 */
struct CallInfo {
public:
  unsigned ID;

  /**
   * @brief A simple call means that the call does not appear as a part or operand
   * of a bigger expression.
   *
   * An example of a simple call:
   * <code>
   * <br> ... C code ...
   * <br> foo(...);
   * <br> ... C code ...
   * </code>
   *
   * An example of a call that is not simple:
   * <code>
   * <br> ... C code ...
   * <br> x = 5 + bar(...);
   * <br> ... C code ...
   * </code>
   */
  bool isSimpleCall;

  /**
   * @brief The location of the begining of the line that contains the call.
   */
  clang::SourceLocation lineStartLoc;

  CallInfo() = default;

  /**
   * @brief Constructor
   *
   * @param ID The ID of the call.
   * @param isSimpleCall Is the call simple?
   *
   * @sa CallInfo::isSimpleCall
   */
  CallInfo(unsigned ID, bool isSimpleCall);

};

/**
 * @brief Stores information about a user defined function.
 */
struct FunctionInfo {
public:
  /**
   * @brief Contains all the calls to the user defined function.
   */
  std::vector<CallInfo> calls;

  /**
   * @brief The name of the referenced function.
   */
  std::string name;

  /**
   * @brief Flag that stores whether the source code of the user defined function
   * should be deleted after the inlining is completed.
   */
  bool deleteSource;

  /**
   * @brief Maps the original names of the variables in the function to the names
   * that these variables will have after the inlining of the function.
   */
  std::map<std::string, std::string> varSubs;

  FunctionInfo() = default;

  /**
   * @brief Constructor
   *
   * @param decl The function declaration
   * @param deleteSource Delete or not the source code of the function after inlining. Defaults to `true`
   * @see http://clang.llvm.org/doxygen/classclang_1_1FunctionDecl.html
   */
  FunctionInfo(const clang::FunctionDecl* decl, bool deleteSource = true);

private:
  /**
   * @brief Locates all the variable declarations in a `clang::Stmt` and stores them
   * in FunctionInfo::varSubs
   *
   * @param  stmt The `clang::Stmt` to search the variables
   */
  void findDeclarations(clang::Stmt* stmt);

};

/**
 * @brief A manager class that aids the communication between the inlining and
 * translation phases
 *
 * @todo Make it a singleton.
 */
class FunctionManager {
public:
  /**
   * @brief Stores the C source code after inlining in order to be used by
   * the translation phase
   */
  std::string fileContents;

  /**
   * @brief Default constructor
   */
  FunctionManager() = default;

  /**
   * @brief Adds a user defined function
   *
   * @param decl The function declaration
   * @see http://clang.llvm.org/doxygen/classclang_1_1FunctionDecl.html
   */
  void addUserFunction(const clang::FunctionDecl* decl);

  /**
   * @brief Checks whether the given function is user defined or not
   *
   * @param name The name of the function
   * @return `true` if the function is user defined, `false` otherwise
   */
  bool isUserDefined(const std::string& name);

  /**
   * @brief Sets whether the source code of the function should be erased after
   * the inlining
   *
   * @param functionName The name of the function
   * @param value `true` to erase and `false` otherwise
   */
  void setDeleteSource(const std::string& functionName, bool value);

  /**
   * @brief Adds a call to a user defined function.
   *
   * @param call The reference to the function call
   * @param isSimpleCall `true` if it is a simple call, `false` otherwise
   * @sa CallInfo::isSimpleCall
   * @see http://clang.llvm.org/doxygen/classclang_1_1CallExpr.html
   */
  void addCall(const clang::CallExpr* call, bool isSimpleCall);

  /**
   * @brief Adds a call to a user defined function.
   *
   * Adds the call with the extra information of the location of the start of the
   * line that contains the call. This is useful in cases that we need to inline
   * a function when it is called as a part of a greater expression.
   *
   * @param call The reference to the function call
   * @param isSimpleCall `true` if it is a simple call, `false` otherwise
   * @param lineLoc The location of the start of the line that contains the call
   * @sa CallInfo::isSimpleCall
   * @see http://clang.llvm.org/doxygen/classclang_1_1CallExpr.html
   */
  void addCall(const clang::CallExpr* call, bool isSimpleCall,
               const clang::SourceLocation& lineLoc);
  /**
   * @brief Checks whether the given call is simple.
   *
   * @param call The reference to the function call
   * @return `true` if the given call is simple, `false` otherwise
   * @sa CallInfo::isSimpleCall
   * @see http://clang.llvm.org/doxygen/classclang_1_1CallExpr.html
   */
  bool isSimpleCall(const clang::CallExpr* call);

  /**
   * @brief Returns the location that a function should be inlined.
   *
   * If the given call is simple call (CallInfo::isSimpleCall) then the inlined
   * code will replace the call. If the given call is not simple then the inlined
   * code will be inserted in the line preceding the statement that contains the
   * call to the function.
   *
   * @param call The reference to the call that is being inlined.
   * @return The location of the code insertion
   * @see http://clang.llvm.org/doxygen/classclang_1_1CallExpr.html
   */
  const clang::SourceLocation getInsertLoc(clang::CallExpr* call);

  /**
   * @brief Returns the variable substitutions for the given function call.
   *
   * @param call The reference to the call that is being inlined.
   * @return A reference to the `std::map` that contains the substitutions.
   * @sa FunctionInfo::varSubs
   * @see http://clang.llvm.org/doxygen/classclang_1_1CallExpr.html
   */
  const std::map<std::string, std::string>& getVarSubs(clang::CallExpr* call);

private:
  /**
   * @brief Holds information about the user defined functions.
   * @sa FunctionInfo
   */
  std::vector<FunctionInfo> userFunctions_;

  FunctionManager(const FunctionManager&) = delete;

  /**
   * @brief Helper function to locate a FunctionInfo instance given the name of
   * the user defined function.
   *
   * @param  name The name of the function.
   * @return An iterator for FunctionManager::userFunctions_
   */
  std::vector<FunctionInfo>::iterator find(const std::string& name);

};

/**
 * @brief A global FunctionManager instance
 */
extern FunctionManager functionMgr;

}

#endif // VYGRAPH_ANALYSISMANAGER_H

/** @file */
