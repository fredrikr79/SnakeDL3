#include "arena.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KB(x) x * 1024

void arena_init(Arena *a, size_t size) {
  *a = (Arena){.buffer = malloc(size), .size = size, .offset = 0};
  if (a->buffer == NULL)
    err(EXIT_FAILURE, "malloc");
}

void *arena_alloc(Arena *a, size_t n, size_t n_size, size_t alignsize) {
  if (alignsize == 0 || (alignsize & (alignsize - 1)) == 1) {
    return NULL;
  }

  size_t allocation_size = n * n_size;

  if (allocation_size < n_size) {
    return NULL;
  }

  size_t total_offset = (size_t)a->offset + (size_t)a->buffer;
  size_t padding = (~total_offset + 1) & (alignsize - 1);

  total_offset += padding;
  if (total_offset + allocation_size > (size_t)a->size + (size_t)a->buffer) {
    return NULL;
  }

  a->offset += (padding + allocation_size);
  memset((void *)total_offset, 0, allocation_size);
  return (void *)total_offset;
}

void arena_reset(Arena *a) {
  a->offset = 0;
  memset(a->buffer, 0, a->size);
}

void arena_free(Arena *a) {
  free(a->buffer);
  a->size = 0;
  a->offset = 0;
  free(a);
}

int main(void) {
  printf("this is the `arena.c` utility main method.\n");

  Arena a = {0};
  arena_init(&a, KB(2));

  int *t = arena_alloc(&a, 6, sizeof(int), _Alignof(int));

  for (int *i = t; i < t + 6; i++) {
    printf("%d ", *i);
  }

  return 0;
}
