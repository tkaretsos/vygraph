#include "CLOptions.hh"

namespace vy {

bool CLOptions::inlineOnly() {
  return toolActionBits->isSet(InlineOnly);
}

bool CLOptions::translateOnly() {
  return toolActionBits->isSet(TranslateOnly);
}

} // namespace vy

/** @file */
