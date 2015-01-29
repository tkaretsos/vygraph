#ifndef VYGRAPH_UTILFUNCTIONS_HH
#define VYGRAPH_UTILFUNCTIONS_HH

#include <string>

namespace clang {

class SourceRange;
class ASTContext;

}

/** @namespace vy */
namespace vy {

/** @namespace vy::util */
namespace util {

/**
 * @brief Converts the source range of a clang::Stmt to std::string.
 *
 * This conversion does not behave correctly when macros are included
 * in the range. More specifically, the use of the SourceManager::getSpellingLoc
 * method depends on whether the range contains a macro.
 *
 * @param range The source range of the text to convert.
 * @param context The current ASTContext.
 * @return A std::string containing the clang::Stmt source range.
 */
std::string range_to_str(const clang::SourceRange& range,
                         const clang::ASTContext& context);

/**
 * @brief Generates a random alphanumeric string of the given length.
 *
 * @param length The length of the generated alphanumeric. Defaults to 3.
 * @return A std::string that contains a random alphanumeric.
 */
std::string random_alphanum(size_t length = 3);

} // namespace util
} // namespace vy

#endif //VYGRAPH_UTILFUNCTIONS_HH

/** @file
 *  @brief Contains helper functions for vygraph.
 */
