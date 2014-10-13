#ifndef VYGRAPH_TRANSLATEASTCONSUMER_HH
#define VYGRAPH_TRANSLATEASTCONSUMER_HH

#include <sstream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclGroup.h"

#include "Translator.hh"

namespace vy {

class TranslateASTConsumer : public clang::ASTConsumer {
  Translator translator;
public:
  TranslateASTConsumer(clang::ASTContext&, std::stringstream&);
  bool HandleTopLevelDecl(clang::DeclGroupRef) override;
};

} // namespace vy

#endif // VYGRAPH_TRANSLATEASTCONSUMER_HH
