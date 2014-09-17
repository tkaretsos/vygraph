
int foo() {
  int c = 1, d = 2, e;
  e = c + d;
  return 5 + e;
}

int bar() {
  return 6;
}

int main() {
  int a;
  a = 3 + foo() + 5;
  return 0;
}
