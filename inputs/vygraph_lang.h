#ifndef VYGRAPH_LANG_H
#define VYGRAPH_LANG_H

/**
 * The following definitions are taken directly from stdbool.h.
 * I had to manually define them, because otherwise the Clang
 * compiler would not generate the AST properly.
 */
#ifndef __cplusplus
#define bool    _Bool
#define true    1
#define false   0
#endif

#ifdef __clang__
  void fork(...)    __attribute__((overloadable));
  void join(...)    __attribute__((overloadable));
#elif __GNUC__
  #define fork(name, ...)                                                      \
    void fork_##name() __attribute__((weak));                                  \
    fork_##name(__VA_ARGS__);

  #define join(name, ...)                                                      \
    void join_##name() __attribute__((weak));                                  \
    join_##name(__VA_ARGS__);
#endif

void vy_atomic_begin()  __attribute__((weak));
void vy_atomic_end()    __attribute__((weak));

void vy_assert(bool)    __attribute__((weak));
void vy_assume(bool)    __attribute__((weak));

#endif /* VYGRAPH_LANG_H */
