#include "vygraph_lang.h"

void sub() {
  int qq = 2, ww = 3;
  qq += 3;
  ww += qq;
}

void add(int s) {
  int ff = 2, gg = 3;
  ff += ff;
  gg += ff;
}

void foo() {
  int a = 2, b = 3;
  a += 3;
  b += a;
}

int ret() {
  int aaa = 2, bbb = 3;
  aaa += 3;
  bbb += aaa;
  return aaa + 10;
}

int main() {
  vy_spawn(sub);
  vy_spawn(add, 5);
  vy_join(sub);
  vy_join(add);
  vy_atomic_begin();
  add(5);
  vy_atomic_end();
  foo();
  ret();
  int aa;
  aa = ret() + 5;
  return 0;
}
