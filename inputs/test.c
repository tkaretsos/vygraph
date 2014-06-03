#include "vygraph_lang.h"

vy_decl_function(add)

void add(int s) {
    int a = 2, b = 3;
    a += 3;
    b += a;
}

int main() {
  vy_spawn(add);
  vy_join(add);
  vy_atomic_begin();
  add(5);
  vy_atomic_end();
  return 0;
}
