#ifndef VYGRAPH_CLOPTIONS_HH
#define VYGRAPH_CLOPTIONS_HH

#include <memory>

#include "llvm/Support/CommandLine.h"

namespace vy {

class CLOptions {
public:
  static CLOptions& Instance() {
    static CLOptions clOptions;
    return clOptions;
  }

  enum ToolActionBit {
    InlineOnly,
    TranslateOnly
  };

  std::unique_ptr<llvm::cl::bits<ToolActionBit>> toolActionBits;

  bool inlineOnly();
  bool translateOnly();

private:
  CLOptions() { }
  CLOptions(const CLOptions&);
  void operator=(const CLOptions&);
};

} // namespace vy

#endif // VYGRAPH_CLOPTIONS_HH
