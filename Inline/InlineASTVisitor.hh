#ifndef VYGRAPH_INLINEASTVISITOR_HH
#define VYGRAPH_INLINEASTVISITOR_HH

#include <map>
#include <vector>

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "../Utility/ClangBaseWrapper.hh"

namespace vy {

class InlineASTVisitor : public clang::RecursiveASTVisitor<InlineASTVisitor> {
  clang::Rewriter& rewriter;
  clang::Rewriter::RewriteOptions opts;
  clang::ASTContext& context;

  void handleSimpleCallNoArgs(clang::CallExpr*,
                              const std::map<std::string, std::string>&) const;
  void handleNoArgs(clang::CallExpr*,
                    const std::map<std::string, std::string>&) const;
  void handleSimpleCallWithArgs(clang::CallExpr*,
                                const std::map<std::string, std::string>&) const;
  void handleArgs(clang::CallExpr*,
                  const std::map<std::string, std::string>&) const;

  void findSubstitutions(clang::Stmt*,
                         std::vector<util::ClangBaseWrapper>&) const;

  void replaceVarsInString(clang::Stmt*, std::string&,
                           const std::map<std::string, std::string>&) const;

  std::string random_alphanum(size_t = 3) const;

public:
  InlineASTVisitor(clang::Rewriter&, clang::ASTContext&);
  bool VisitCallExpr(clang::CallExpr*);
};

} // namespace vy

#endif // VYGRAPH_INLINEASTVISITOR_HH
