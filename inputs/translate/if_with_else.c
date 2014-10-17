#include "../vygraph_lang.h"

int y = 1;
int k;

int main () {
  bool b = false;
  bool a = true;
  bool c;
  int x = 0;
  if (b) {
    x = x + 3;
    x++;
  } else {
    x = x - 3;
    x = 2 * x;
  }
  x--;
  return 0;
}
