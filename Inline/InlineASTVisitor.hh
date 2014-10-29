#ifndef VYGRAPH_INLINEASTVISITOR_HH
#define VYGRAPH_INLINEASTVISITOR_HH

#include <map>
#include <vector>

#include "clang/AST/RecursiveASTVisitor.h"
#include "Utility/ClangBaseWrapper.hh"

namespace vy {

class InlineASTVisitor : public clang::RecursiveASTVisitor<InlineASTVisitor> {
public:
  InlineASTVisitor(clang::Rewriter&, clang::ASTContext&);
  bool VisitCallExpr(clang::CallExpr*);

private:
  clang::Rewriter& rewriter;
  clang::ASTContext& context;

  void noArgsNoRet(clang::CallExpr*,
                   const std::map<std::string, std::string>&) const;
  void noArgsWithRet(clang::CallExpr*,
                     const std::map<std::string, std::string>&) const;
  void argsNoRet(clang::CallExpr*,
                 const std::map<std::string, std::string>&) const;
  void argsWithRet(clang::CallExpr*,
                   const std::map<std::string, std::string>&) const;

  void findSubstitutions(clang::Stmt*,
                         std::vector<util::ClangBaseWrapper>&) const;
  void replaceVarsInString(clang::Stmt*, std::string&,
                           const std::map<std::string, std::string>&) const;
};

} // namespace vy

#endif // VYGRAPH_INLINEASTVISITOR_HH
