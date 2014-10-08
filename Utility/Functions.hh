#ifndef VYGRAPH_UTILFUNCTIONS_HH
#define VYGRAPH_UTILFUNCTIONS_HH

#include <string>

#include "clang/AST/ASTContext.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"

namespace vy {
namespace util {

std::string
RangeToStr(const clang::SourceRange& range, const clang::ASTContext& context) {
  auto& SM = context.getSourceManager();
  auto charRange = clang::CharSourceRange::getTokenRange(range);
  auto beginLoc = SM.getSpellingLoc(charRange.getBegin());
  auto endLoc = SM.getSpellingLoc(charRange.getEnd());
  auto size = SM.getDecomposedLoc(endLoc).second -
              SM.getDecomposedLoc(beginLoc).second;
  size += clang::Lexer::MeasureTokenLength(endLoc, SM, context.getLangOpts());

  return std::string(SM.getCharacterData(beginLoc), size);
}

} // namespace util
} // namespace vy

#endif //VYGRAPH_UTILFUNCTIONS_HH
