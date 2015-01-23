#ifndef VYGRAPH_CLOPTIONS_HH
#define VYGRAPH_CLOPTIONS_HH

#include <memory>

#include "llvm/Support/CommandLine.h"

namespace vy {

class CLOptions {
public:
  enum ToolActionBit {
    InlineOnly,
    TranslateOnly
  };

  std::unique_ptr<llvm::cl::bits<ToolActionBit>> toolActionBits;

  static CLOptions& Instance() {
    static CLOptions clOptions;
    return clOptions;
  }

  bool inlineOnly();
  bool translateOnly();

private:
  CLOptions() = default;
  CLOptions(const CLOptions&) = delete;
  void operator=(const CLOptions&) = delete;

};

} // namespace vy

#endif // VYGRAPH_CLOPTIONS_HH
