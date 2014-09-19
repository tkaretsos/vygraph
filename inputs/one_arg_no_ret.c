
void foo(int x, int y) {
  int z = 5 + x + y;
  return;
}

int main() {
  int a = 5;
  foo(a + 3 + 4 + 5, a);

  return 0;
}
