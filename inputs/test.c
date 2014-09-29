#include "vygraph_lang.h"

void sub() {
  int x_sub = 2, y_sub = 3;
  x_sub += 3;
  y_sub += x_sub;
}

void add(int x_add) {
  int y_add = 2, z_add = 3;
  y_add += x_add;
  z_add += x_add;
}

void foo() {
  int x_foo = 2, y_foo = 3;
  x_foo += 3;
  y_foo += x_foo;
}

int ret(int x_ret) {
  int y_ret = 3;
  x_ret = 2;
  x_ret += 3;
  y_ret += x_ret;
  return x_ret + 10;
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
  int x;
  x = ret(x + 4) + 5;
  return 0;
}
