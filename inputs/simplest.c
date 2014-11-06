#include "vygraph_lang.h"

bool read;
bool barrierOpen = false;
int waiting = 0;
int capacity = 0;
bool enough = false;

void thread() {
  read = true;
  read = false;
  waiting = waiting + 1;

  vy_atomic_begin();
  vy_assume(enough);
  vy_assume(waiting == capacity);
  barrierOpen = true;
  waiting = waiting - 1;
  vy_atomic_end();

  vy_atomic_begin();
  vy_assume(barrierOpen);
  waiting = waiting - 1;
  vy_atomic_end();

  vy_assume(read);
}

int main() {
  vy_atomic_begin();
  vy_assume(!enough);
  fork(thread);
  capacity = capacity + 1;
  vy_atomic_end();
  enough = false;
  enough = true;

  return 0;
}
