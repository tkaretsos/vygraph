
void foo() {
  int x_foo = 3;
  int y_foo = 2, z_foo;
  z_foo = x_foo + y_foo;
}

void bar() {
  int x_bar = 3;
  int y_bar = 2, z_bar;
  z_bar = x_bar + y_bar;
  return;
}

int main() {
  int x, y;
  x = y = 3;
  // 1
  foo();
  // 2
  bar();
  // 3
  bar();

  return 0;
}
