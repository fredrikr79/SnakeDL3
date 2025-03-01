#ifndef ARENA_H_
#define ARENA_H_

#include <stddef.h>

typedef struct {
  char *buffer;
  size_t size;
  int offset;
} Arena;

void arena_init(Arena *a, size_t size);

void *arena_alloc(Arena *a, size_t n, size_t n_size, size_t alignsize);

void arena_reset(Arena *a);

void arena_free(Arena *a);

#endif // ARENA_H_
