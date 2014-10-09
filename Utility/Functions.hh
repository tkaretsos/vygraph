#ifndef VYGRAPH_UTILFUNCTIONS_HH
#define VYGRAPH_UTILFUNCTIONS_HH

#include <string>

#include "clang/AST/ASTContext.h"
#include "clang/Basic/SourceLocation.h"

namespace vy {
namespace util {

std::string RangeToStr(const clang::SourceRange&, const clang::ASTContext&);

std::string random_alphanum(size_t = 3);

} // namespace util
} // namespace vy

#endif //VYGRAPH_UTILFUNCTIONS_HH
