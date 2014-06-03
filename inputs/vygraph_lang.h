#ifndef VYGRAPH_LANG_H
#define VYGRAPH_LANG_H

#ifdef __clang__
  void vy_spawn(...) __attribute__((overloadable));
  void vy_join(...) __attribute__((overloadable));
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
