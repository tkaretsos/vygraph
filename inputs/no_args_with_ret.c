
int foo() {
  int x_foo = 3;
  int y_foo = 5;
  return 4 + x_foo + y_foo;
}

float bar() {
  float x_bar = 3.3;
  float y_bar;
  y_bar = 3 * 3.5 + x_bar;
  return y_bar;
}

int main() {
  int x, y;
  // 1
  x = 3 * foo() + 4;
  // 2
  foo();
  // 3
  foo();
  // 4
  bar();
  // 5
  float z = 5.2 + bar();

  return 0;
}
