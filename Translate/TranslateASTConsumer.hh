#ifndef VYGRAPH_TRANSLATEASTCONSUMER_HH
#define VYGRAPH_TRANSLATEASTCONSUMER_HH

#include <sstream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclGroup.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Rewrite/Core/Rewriter.h"

namespace vy {

class TranslateASTConsumer : public clang::ASTConsumer {
  clang::ASTContext& context;
  std::stringstream& outs;

  std::string getAsString(const clang::SourceRange&);

  void insertVarDecl(clang::VarDecl*);

public:
  TranslateASTConsumer(clang::ASTContext&, std::stringstream&);
  bool HandleTopLevelDecl(clang::DeclGroupRef) override;
};

} // namespace vy

#endif // VYGRAPH_TRANSLATEASTCONSUMER_HH
