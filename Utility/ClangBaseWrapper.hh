#ifndef VYGRAPH_UTIL_CLANGBASEWRAPPER_H
#define VYGRAPH_UTIL_CLANGBASEWRAPPER_H

#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Rewrite/Core/Rewriter.h>

namespace vy {
namespace util {

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
