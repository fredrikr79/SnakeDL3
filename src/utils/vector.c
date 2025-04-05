#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

void vector_init(Vector *v, int capacity) {
  v->data = malloc(capacity * sizeof(int));
  if (!v->data) {
    printf("vector_init: failed to allocate data\n");
    return;
  }
  v->size = 0;
  v->_capacity = capacity;
}

void vector_grow(Vector *v) {
  v->_capacity <<= 1;
  v->data = realloc(v->data, sizeof(int) * v->_capacity);
  if (!v->data) {
    printf("vector_grow: failed to allocate data\n");
    return;
  }
}

void vector_push(Vector *v, int val) {
  if (v->size >= v->_capacity) {
    vector_grow(v);
  }
  v->data[v->size] = val;
  v->size++;
}

void vector_pusha(Vector *v, int *vals, int n) {
  for (int i = 0; i < n; i++) {
    int val = vals[i];
    vector_push(v, val);
  }
}

int vector_pop(Vector *v) {
  if (v->size < 1) {
    printf("vector_pop: popping empty vector");
    return 0;
  }
  int val = v->data[v->size - 1];
  v->size--;
  return val;
}

void vector_free(Vector *v) {
  free(v->data);
  v->size = 0;
  v->_capacity = 0;
}

void vector_print(Vector v) {
  printf("vector_print: ");
  for (int i = 0; i < v.size; i++) {
    printf("%d ", v.data[i]);
  }
  printf("\n");
}

// int main(int argc, char *argv[]) {
//   Vector *v = vector_new();
//   vector_push(v, 20);
//   vector_pusha(v, (int[]){1, 2, 3}, 3);
//   vector_print(*v);
//   printf("oops, i removed %d\n", vector_pop(v));
//   vector_print(*v);
//   vector_free(v);
//   return 0;
// }
