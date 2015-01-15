#include "../vygraph_lang.h"

int y = 1;
int k;

int main () {
  bool b = false;
  bool a = true;
  bool c;
  int x = 0;

  if (b) {
    x = x + 1;
    x = x + 2;
    zomg:
    x = x + 10;
  }

  return 0;
}
