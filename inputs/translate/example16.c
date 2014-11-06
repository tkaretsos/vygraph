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
    vy_atomic_begin();
    x++;
    vy_atomic_end();
    vy_assert(x != 0);
  } else {
    x = x - 3;
    vy_assume(x == 3);
    x = 2 * x;
    x += 3;
  }
  vy_atomic_begin();
  x--;
  vy_atomic_end();
  return 0;
}
