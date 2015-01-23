#ifndef VYGRAPH_UTILFUNCTIONS_HH
#define VYGRAPH_UTILFUNCTIONS_HH

#include <string>

namespace clang {

class SourceRange;
class ASTContext;

}

namespace vy {
namespace util {

std::string range_to_str(const clang::SourceRange& range,
                         const clang::ASTContext& context);

std::string random_alphanum(size_t = 3);

} // namespace util
} // namespace vy

#endif //VYGRAPH_UTILFUNCTIONS_HH
