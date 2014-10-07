#include "vygraph_lang.h"
#include <stdbool.h>
#include <assert.h>

bool lock = true;
bool writing = false;
int i = 0;

void writer() {
  writing = true;
  writing = false;
}

void reader() {
  assert(!writing);
}

void increment_i() {
  i = i + 1;
}

int main () {
  vy_atomic_begin();
//     assume(lock);
  lock = false;
  vy_spawn(writer);
  vy_atomic_end();

  vy_atomic_begin();
  vy_join(writer);
  lock = true;
  vy_atomic_end();

  if (i == 0) {
    vy_atomic_begin();
    increment_i();
//       assume(lock);
    lock= false;
    vy_spawn(reader);
    vy_atomic_end();
  } else if (i >= 1) {
    vy_atomic_begin();
    increment_i();
    vy_spawn(reader);
    vy_atomic_end();
  }

  return 0;
}
