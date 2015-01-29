#ifndef VYGRAPH_CLOPTIONS_HH
#define VYGRAPH_CLOPTIONS_HH

#include <memory>

#include "llvm/Support/CommandLine.h"

namespace vy {

/**
 * @brief A class to manage the command line arguments to vygraph.
 *
 * Currently only two options are supported. The `-inline-only` and the
 * `-translate-only`.
 */
class CLOptions {
public:
  /**
   * @brief Enum used for `llvm::cl::bits<T>` class.
   */
  enum ToolActionBit {
    InlineOnly,     /**< represents whether the `-inline-only` option is given */
    TranslateOnly   /**< represents whether the `-translate-only` option is given */
  };

  /**
   * @brief Stores the information about the command line arguments given to
   * vygraph.
   *
   * @see http://llvm.org/docs/CommandLine.html#the-cl-bits-class
   */
  std::unique_ptr<llvm::cl::bits<ToolActionBit>> toolActionBits;

  static CLOptions& Instance() {
    static CLOptions clOptions;
    return clOptions;
  }

  /**
   * @brief Checks if the `-inline-only` option was given.
   *
   * @return `true` if the `-inline-only` option was given and `false` otherwise.
   */
  bool inlineOnly();

  /**
   * @brief Checks if the `-translate-only` option was given.
   *
   * @return `true` if the `-translate-only` option was given and `false` otherwise.
   */
  bool translateOnly();

private:
  CLOptions() = default;
  CLOptions(const CLOptions&) = delete;
  void operator=(const CLOptions&) = delete;

};

} // namespace vy

#endif // VYGRAPH_CLOPTIONS_HH

/** @file */
