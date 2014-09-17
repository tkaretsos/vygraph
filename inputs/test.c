#include "vygraph_lang.h"

void sub() {
  int a = 2, b = 3;
  a += 3;
  b += a;
}

void add(int s) {
  int a = 2, b = 3;
  a += s;
  b += s;
}

void foo() {
  int a = 2, b = 3;
  a += 3;
  b += a;
}

int ret() {
  int a = 2, b = 3;
  a += 3;
  b += a;
  return a + 10;
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
  int a;
  a = ret() + 5;
  return 0;
}
