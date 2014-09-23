
int foo(int x) {
  int y = x + 1;
  return y;
}

float bar(float x, float y) {
  float z = x * y;
  return z + 1;
}

int main() {
  int a = 5;
  a = 5 + foo(a)  * 6;

  float f = 1.0f + bar(3.f + 4.f, 6.f) * 3;

  return 0;
}
