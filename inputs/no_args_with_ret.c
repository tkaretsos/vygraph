
int foo() {
  int x_foo = 3;
  int y_foo = 5, z_foo = 3;
  x_foo += 3 * y_foo;
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
  float z = 5.2 + bar();

  return 0;
}
