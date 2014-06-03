#ifndef VYGRAPH_LANG_H
#define VYGRAPH_LANG_H

#define vy_decl_function(name) \
\
void spawn_thread_##name();\
\
void join_thread_##name();
/* end vy_decl_function macro */

#define vy_spawn(function, ...) spawn_thread_##function();
#define vy_join(function, ...) join_thread_##function();

void vy_atomic_begin();
void vy_atomic_end();

#endif /* VYGRAPH_LANG_H */
