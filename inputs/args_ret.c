
int foo(int x_foo, int y_foo) {
  int z_foo = x_foo * y_foo + 3;
  z_foo = x_foo + 5 - y_foo + 3;
  z_foo = x_foo + 2 * y_foo;
  z_foo = x_foo + y_foo;
  z_foo = x_foo + y_foo;
  return z_foo + 5 + x_foo;
}

float bar(float x_bar, float y_bar) {
  float z_bar = 3.1;
  z_bar *= x_bar + y_bar;
  return z_bar;
}

int main() {
  int x = 5;
  int y = 6;
  // 1
  int z = foo(x, y);
  // 2
  int w = foo(x + 6, y);
  // 3
  int v = foo(x + y, 3);
  // 4
  int h = 6 * foo(3, 5) + 5;
  // 5
  float f = bar(3.3, 6);
  // 6
  float o = 3.2;
  // 7
  float l = z * bar(z, z);

  return 0;
}
