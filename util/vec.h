#ifndef AOC_VEC_H
#define AOC_VEC_H

#include <stdbool.h>
#include <stddef.h>

struct vec;
typedef struct vec vec_t;

typedef void *(*vec_vcpy)(void *in);

typedef void (*vec_vfree)(void *in);

typedef struct {
    vec_vcpy vcpy;
    vec_vfree vfree;
} vec_callbacks;

vec_t *vec_create(vec_callbacks *cbs);

vec_t *vec_cpy(vec_t *vec);

void vec_destroy(vec_t *vec);

bool vec_push_front(vec_t *vec, void *val);

bool vec_push(vec_t *vec, void *val);

bool vec_remove(vec_t *vec, size_t i);

void *vec_pop(vec_t *vec);

void *vec_peek(vec_t *vec);

void *vec_pop_front(vec_t *vec);

size_t vec_len(vec_t *vec);

bool vec_set(vec_t *vec, size_t i, void *newval);

void *vec_get(vec_t *vec, size_t i);

void vec_qsort(vec_t *vec, int (*compar)(const void *, const void *));
void *vec_lfind(vec_t *vec, const void *key, int (*compar)(const void *, const void *));
void *vec_bsearch(vec_t *vec, const void *key, int (*compar)(const void *, const void *));

size_t vec_collect(vec_t *vec, void ***save);

#endif//AOC_VEC_H
