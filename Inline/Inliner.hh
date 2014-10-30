#ifndef VYGRAPH_INLINER_HH
#define VYGRAPH_INLINER_HH

#include <map>
#include <vector>

namespace clang {

class ASTContext;
class CallExpr;
class Rewriter;
class SourceLocation;
class Stmt;

}

namespace vy {

namespace util {
  class ClangBaseWrapper;
}

class Inliner {
public:
  Inliner(const clang::ASTContext&, clang::Rewriter&);

  void init(clang::CallExpr*);
  void doInline();

  void inlineStmt(const clang::Stmt*, std::string&);
  void inlineReturnStmt(std::string&);
  void inlineArguments();
  void inlineBody();

  void deleteCallText();

  void findSubstitutions(clang::Stmt*, std::vector<util::ClangBaseWrapper>&);
  void replaceVarsInString(clang::Stmt*, std::string&);

private:
  const clang::ASTContext& context;
  clang::Rewriter& rewriter;
  clang::CallExpr* call;
  std::map<std::string, std::string> subMap;
};

} // namespace vy

#endif // VYGRAPH_INLINER_HH
