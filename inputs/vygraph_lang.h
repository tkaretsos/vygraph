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
  void vy_spawn(...)    __attribute__((overloadable));
  void vy_join(...)     __attribute__((overloadable));
#elif __GNUC__
  #define vy_spawn(name, ...)                                                  \
    void vy_spawn_##name() __attribute__((weak));                              \
    vy_spawn_##name(__VA_ARGS__);

  #define vy_join(name, ...)                                                   \
    void vy_join_##name() __attribute__((weak));                               \
    vy_join_##name(__VA_ARGS__);
#endif

void vy_atomic_begin() __attribute__((weak));
void vy_atomic_end() __attribute__((weak));

#endif /* VYGRAPH_LANG_H */
