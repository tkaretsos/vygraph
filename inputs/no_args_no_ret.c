
void foo() {
  int x_foo = 3;
  int y_foo = 2, z_foo;
  z_foo = x_foo + y_foo;
  {
    int x_foo = 5;
    z_foo += x_foo;
  }
  if (z_foo == y_foo) {
    z_foo++;
  } else {
    z_foo--;
  }
  for (int i = 0; i < 5; i++) {
    x_foo++;
  }
  int j = 0;
  while (j < 10) {
    j++;
  }
  int k = 0;
  do {
    k++;
  } while (k < 10);
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
