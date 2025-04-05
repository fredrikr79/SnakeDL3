#ifndef VECTOR_H
#define VECTOR_H

#define VEC_MIN 8

#define vector_new(v) vector_init(v, VEC_MIN)

typedef struct {
  int *data;
  int size;
  int _capacity;
} Vector;

void vector_init(Vector *v, int capacity);

void vector_grow(Vector *v);

void vector_push(Vector *v, int val);

void vector_pusha(Vector *v, int *vals, int n);

int vector_pop(Vector *v);

void vector_free(Vector *v);

void vector_print(Vector v);

#endif // VECTOR_H
