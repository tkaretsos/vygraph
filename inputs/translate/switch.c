#include "../vygraph_lang.h"

int main() {
  int x = 5;
  switch (x) {
    case 1:
      x = 11;
      break;

    case 2:
      x = 22;
      break;

    case 3:
      x = 33;
      break;

    default:
      x = 5;
  }

  return 0;
}
