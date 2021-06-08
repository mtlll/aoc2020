//
// Created by mtl on 19/05/2021.
//

#ifndef AOC_NDARRAY_H
#define AOC_NDARRAY_H

#include <stddef.h>
#include <stdbool.h>

struct ndarray;
typedef struct ndarray ndarray_t;
typedef size_t *ndarray_idx_t;

struct nda_enum;
typedef struct nda_enum nda_enum_t;

void print_idx(ndarray_t *nda, ndarray_idx_t idx);
ndarray_t *nda_create(size_t rank, size_t membsize, ...);
void nda_destroy(ndarray_t *nda);

size_t nda_rank(ndarray_t *nda);
size_t nda_extent(ndarray_t *nda, size_t dim);
void *nda_get(ndarray_t *nda, ndarray_idx_t idx);
void *nda_set(ndarray_t *nda, ndarray_idx_t idx, void *elem);

void nda_set_all(ndarray_t *nda, void *elem);

nda_enum_t *nda_enum_create(ndarray_t *nda, size_t offset, ndarray_idx_t start);
void nda_enum_destroy(nda_enum_t *nde);
size_t nde_rank(nda_enum_t *nde);
void nde_neighbour_enum(nda_enum_t *nde);
bool neighbour_behind (nda_enum_t *nde);
bool nda_enum_next(nda_enum_t *nde);
void *nda_enum_get(nda_enum_t *nde, ndarray_t *nda);
void *nda_enum_set(nda_enum_t *nde, ndarray_t *nda, void *elem);


#endif //AOC_NDARRAY_H