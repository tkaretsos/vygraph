#include "CLOptions.hh"

using namespace llvm::cl;

namespace vy {

bool CLOptions::inlineOnly() {
  return toolActionBits->isSet(InlineOnly);
}

bool CLOptions::translateOnly() {
  return toolActionBits->isSet(TranslateOnly);
}

} // namespace vy
