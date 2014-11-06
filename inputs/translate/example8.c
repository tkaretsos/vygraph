#include "../vygraph_lang.h"

int y = 1;
int k;

int main () {
  bool b = false;
  bool a = true;
  bool c;
  int x = 0;
  if (x == 0) {
    x = x + 3;
    x++;
  } else {
    x = x - 3;
    if (a) {
      x = 2;
    } else {
      x = 3;
    }
  }
  x--;
  return 0;
}
