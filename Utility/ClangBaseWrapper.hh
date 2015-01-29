#ifndef VYGRAPH_UTIL_CLANGBASEWRAPPER_H
#define VYGRAPH_UTIL_CLANGBASEWRAPPER_H

#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Rewrite/Core/Rewriter.h>

namespace vy {
namespace util {

/**
 * @brief Wrapper class of `clang::Decl` and `clang::Stmt` base classes.
 *
 * Makes easier the use of the common methods of `clang::Decl` and `clang::Stmt`
 * classes.
 *
 * @see http://clang.llvm.org/doxygen/classclang_1_1Decl.html
 * @see http://clang.llvm.org/doxygen/classclang_1_1Stmt.html
 */
class ClangBaseWrapper {
public:
  ClangBaseWrapper(const clang::Stmt* stmt) :
    stmtPtr(stmt), namedDeclPtr(nullptr)
  { }

  ClangBaseWrapper(const clang::NamedDecl* namedDecl) :
    stmtPtr(nullptr), namedDeclPtr(namedDecl)
  { }

  clang::SourceLocation getLocStart() const {
    return (stmtPtr) ? stmtPtr->getLocStart() : namedDeclPtr->getLocation();
  }

  clang::SourceLocation getLocEnd() const {
    return (stmtPtr) ? stmtPtr->getLocEnd() : namedDeclPtr->getLocEnd();
  }

  clang::SourceRange getSourceRange() const {
    return (stmtPtr) ? stmtPtr->getSourceRange() : namedDeclPtr->getSourceRange();
  }

  std::string getAsString(clang::Rewriter& rewriter) const {
    if (stmtPtr)
      return rewriter.ConvertToString(const_cast<clang::Stmt*>(stmtPtr));
    else
      return namedDeclPtr->getDeclName().getAsString();
  }

private:
  const clang::Stmt* stmtPtr;
  const clang::NamedDecl* namedDeclPtr;

  ClangBaseWrapper() = delete;

};

} // namespace util
} // namespace vy

#endif

/** @file */
