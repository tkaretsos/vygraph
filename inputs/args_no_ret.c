#include <stdio.h>

void foo(int x_foo, int y_foo) {
  int z_foo = x_foo * y_foo + 3;
  z_foo = x_foo + 5 - y_foo + 3;
  z_foo = x_foo + 2 * y_foo;
  if (z_foo == x_foo) {
    z_foo = x_foo + y_foo;
    z_foo--;
  }else{
    z_foo++;
    printf("%d\n", z_foo);
  }
  for (int i = 0; i < 5; ++i) {
    z_foo += i;
  }
//   int j = 0;
//   while (j < 5)
//   {
//     z_foo -= j;
//     j++;
//   }
//   int k = 0;
//   do {
//     z_foo += k;
//     k++;
//   } while (k < 6);
  z_foo++;
}

void bar(float x_bar, float y_bar) {
  float z_bar = 3.1;
  z_bar *= x_bar + y_bar;
}

int main() {
  int x = 5;
  int y = 6;
  // 1
  foo(x, y);
  // 2
  foo(x + 6, y);
  // 3
  foo(x + y, 3);
  // 4
  foo(3, 5);
  // 5
  bar(3.3, 6);
  // 6
  float z = 3.2;
  // 7
  bar(z, z);

  return 0;
}
