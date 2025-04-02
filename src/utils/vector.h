#ifndef VECTOR_H
#define VECTOR_H

#define VEC_MIN 8

typedef struct {
  int *data;
  int size;
  int _capacity;
} Vector;

void vector_init(Vector **v, int capacity);

Vector *vector_new(void);

void vector_grow(Vector *v);

void vector_push(Vector *v, int val);

int vector_pop(Vector *v);

void vector_free(Vector *v);

void vector_print(Vector v);

#endif // VECTOR_H
