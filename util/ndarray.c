#include "ndarray.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "../flags.h"

struct ndarray {
   size_t rank;
   size_t membsize;
   size_t *extents;
   size_t nelems;
   void *data;
   size_t nbehind;
};

struct nda_enum {
    ndarray_t *nda;
    size_t offset;
    ndarray_idx_t cur;
    size_t cur_elem;
    ndarray_idx_t curn;
    size_t nelem;
    bool isbehind;
    size_t nbehind;
};

static size_t compute_idx(ndarray_t *nda, ndarray_idx_t idx)
{
	size_t ielem = 0;
	size_t i;

	for (i = 0; i < nda->rank - 1; i += 1) {
		ielem += idx[i];
		ielem *= nda->extents[i + 1];
	}
	ielem += idx[i];

	return ielem;
}

static inline void *nda_idx_ptr(ndarray_t *nda, size_t idx)
{

	return (void *) ((char *) nda->data + idx * nda->membsize);
}

void print_idx(ndarray_t *nda, ndarray_idx_t idx)
{
	size_t i;
	printf("(");
	for (i = 0; i < nda->rank - 1; i += 1) {
		printf("%lu, ", idx[i]);
	}

	printf("%lu) = %lu", idx[i], compute_idx(nda, idx));
}
static ndarray_t *_nda_create(size_t rank, size_t membsize, size_t *extents)
{
	ndarray_t *nda = malloc(sizeof (ndarray_t));
	nda->rank = rank;
	nda->membsize = membsize;
	nda->extents = extents;
	nda->nelems = 1;
	nda->nbehind = 1;
	for (size_t i = 0; i < rank; i += 1) {
		nda->nelems *= extents[i];
		nda->nbehind *= 3;
	}
	nda->nbehind >>= 1;

	nda->data = calloc(nda->nelems, membsize);

	return nda;
}

ndarray_t *nda_create(size_t rank, size_t membsize, ...)
{
	va_list ap;
	size_t *extents = reallocarray(NULL, rank, sizeof (size_t));

	va_start(ap, membsize);

	for (size_t i = 0; i < rank; i += 1) {
		extents[i] = va_arg(ap, size_t);
	}

	va_end(ap);

	return _nda_create(rank, membsize, extents);
}

void nda_destroy(ndarray_t *nda)
{
	free(nda->extents);
	free(nda->data);
	free(nda);
}


size_t nda_rank(ndarray_t *nda)
{
	if (nda == NULL) {
		return 0;
	} else {
		return nda->rank;
	}
}
size_t nda_extent(ndarray_t *nda, size_t dim)
{
	if (dim >= nda->rank) {
		return 0;
	} else {
		return nda->extents[dim];
	}
}

void *nda_get(ndarray_t *nda, ndarray_idx_t idx)
{
	return nda_idx_ptr(nda, compute_idx(nda, idx));
}

static void *_nda_set(ndarray_t *nda, size_t idx, void *elem)
{
	void *dest = nda_idx_ptr(nda, idx);
	memcpy(dest, elem, nda->membsize);
	return dest;
}

void *nda_set(ndarray_t *nda, ndarray_idx_t idx, void *elem)
{
	return _nda_set(nda, compute_idx(nda, idx), elem);
}

void nda_set_all(ndarray_t *nda, void *elem)
{
	nda_enum_t *nde = nda_enum_create(nda, 0, NULL);

	do {
		nda_enum_set(nde, NULL, elem);
	} while (nda_enum_next(nde));

	nda_enum_destroy(nde);
}

nda_enum_t *nda_enum_create(ndarray_t *nda, size_t offset, ndarray_idx_t start)
{
	nda_enum_t *ret = calloc(1, sizeof (nda_enum_t));
	ret->nda = nda;
	ret->offset = offset;
	if (start) {
		ret->cur = start;
		ret->cur_elem = compute_idx(nda, ret->cur);
	} else {
		ret->cur = calloc(nda->rank, sizeof (size_t));
		if (offset) {
			for (size_t i = 0; i < nda->rank; i += 1) {
				ret->cur[i] = offset;
			}
			ret->cur_elem = compute_idx(nda, ret->cur);
		}
	}

	return ret;
}

void nda_enum_destroy(nda_enum_t *nde)
{
	free(nde->cur);
	free(nde);
}

size_t nde_rank(nda_enum_t *nde)
{
	if (nde == NULL) {
		return 0;
	} else {
		return nde->nda->rank;
	}
}

void nde_neighbour_enum(nda_enum_t *nde)
{
	ndarray_t *nda = nde->nda;
	ndarray_idx_t curn = reallocarray(NULL, nda->rank, sizeof (size_t));
	for (size_t i = 0; i < nda->rank; i += 1) {
		curn[i] = (nde->cur[i] > nde->offset) ? nde->cur[i] - 1 : nde->cur[i];
	}

	nde->curn = curn;
	nde->nelem = compute_idx(nda, curn);
	nde->isbehind = true;
}
bool neighbour_behind(nda_enum_t *nde)
{
	if (nde == NULL || nde->curn == NULL) {
		return false;
	}

	return nde->isbehind;
}


static bool nde_inc_cur(nda_enum_t *nde)
{
	ndarray_t *nda = nde->nda;
	size_t n = nda->rank;

	while (n--) {
		nde->cur[n] += 1;
		nde->cur[n] %= nda->extents[n] - nde->offset;
		if (nde->cur[n]) {
			if (nde->offset && n < nda->rank - 1) {
				nde->cur_elem = compute_idx(nda, nde->cur);
			} else {
				nde->cur_elem += 1;
			}
			return true;
		} else {
			nde->cur[n] = nde->offset;
		}
	}

	free(nde->cur);
	nde->cur = NULL;
	return false;
}

static bool nde_next_neighbour(nda_enum_t *nde) {

	ndarray_t *nda = nde->nda;
	size_t n = nda->rank;

	ndarray_idx_t cur = nde->cur;
	ndarray_idx_t curn = nde->curn;
	size_t *ext = nda->extents;
	bool ret = false;

	while (n--) {
		if (curn[n] > cur[n]) {
			curn[n] -= (cur[n] > nde->offset) ? 2 : 1;
		} else if (curn[n] == ext[n] - 1 - nde->offset) {
			curn[n] -= 1;
		} else {
			curn[n] += 1;
			ret = true;
			if (n < nda->rank - 1) {
				nde->nelem = compute_idx(nda, curn);
			} else {
				nde->nelem += 1;
			}
			break;
		}
	}

	if (ret) {
		if (nde->isbehind)  {
			if (!memcmp(cur, curn, sizeof (size_t[nda->rank]))) {
				nde->isbehind = false;
				return nde_next_neighbour(nde);
			}
		}
	} else {
		free(nde->curn);
		nde->curn = NULL;
	}
	return ret;
}

bool nda_enum_next(nda_enum_t *nde)
{
	if (nde->curn) {
		return nde_next_neighbour(nde);
	} else if (nde->cur) {
		return nde_inc_cur(nde);
	} else {
		return false;
	}
}

void *nda_enum_get(nda_enum_t *nde, ndarray_t *nda)
{
	if (nda == NULL) {
		nda = nde->nda;
	}

	if (nde->curn) {
		return nda_idx_ptr(nda, nde->nelem);
	} else if (nde->cur) {
		return nda_idx_ptr(nda, nde->cur_elem);
	} else {
		return NULL;
	}
}

void *nda_enum_set(nda_enum_t *nde, ndarray_t *nda, void *elem)
{
	if (nda == NULL) {
		nda = nde->nda;
	}

	if (nde->curn) {
		return _nda_set(nda, nde->nelem, elem);
	} else if (nde->cur) {
		return _nda_set(nda, nde->cur_elem, elem);
	} else {
		return NULL;
	}
}