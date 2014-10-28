int main() {
  int x = 0;
  int y = 0;
  while (x > 5) {
    if (y <= 20) {
      while (x <= y) {
        x++;
        y--;
      }
    }
    x--;
  }
  return 0;
}
