#include <stdlib.h>
#include <string.h>
//#include <search.h>

#include "vec.h"

struct vec {
    void **data;
    size_t nmemb;
    size_t nused;
    size_t offset;
    vec_callbacks cbs;
};

static void *vec_passthrough_cpy(void *v)
{
	return v;
}

static void vec_passthrough_destroy(void *v)
{
	return;
}

static const size_t MEMB_START = 256;

vec_t *vec_create(vec_callbacks *cbs)
{
	vec_t *vec;

	vec = calloc(1, sizeof(vec_t));
	vec->nmemb = MEMB_START;
	vec->offset = vec->nmemb / 2;
	vec->cbs.vcpy = vec_passthrough_cpy;
	vec->cbs.vfree = vec_passthrough_destroy;

	if (cbs != NULL) {
		if (cbs->vcpy != NULL) vec->cbs.vcpy = cbs->vcpy;
		if (cbs->vfree != NULL) vec->cbs.vfree = cbs->vfree;
	}

	vec->data = reallocarray(NULL, vec->nmemb, sizeof(void *));

	return vec;
}
static void **vec_elem(vec_t *vec, size_t i)
{
	void **ret = vec->data + vec->offset + i;
	return ret;
}

vec_t *vec_cpy(vec_t *vec)
{
	vec_t *newvec = vec_create(&vec->cbs);
	newvec->nmemb = vec->nmemb;
	newvec->data = reallocarray(newvec->data, newvec->nmemb, sizeof (void *));

	for (size_t i = 0; i < vec->nused; i += 1) {
		vec_push(newvec, vec_get(vec, i));
	}

	return newvec;
}

void vec_destroy(vec_t *vec)
{
	size_t i;

	if (vec == NULL) {
		return;
	}
	for (i = 0; i < vec->nused; i += 1) {
		vec->cbs.vfree(*vec_elem(vec, i));
	}
	free(vec->data);
	free(vec);
}

static bool vec_grow(vec_t *vec)
{
	size_t nmemb = vec->nmemb * 4;
	size_t offset = nmemb / 2;

	void **new = reallocarray(NULL, nmemb, sizeof(void *));

	if (new == NULL) {
		return false;
	}

	if (vec->nused > 0) {
		memcpy(new + offset, vec_elem(vec, 0), sizeof (size_t[vec->nused]));
	}
	free(vec->data);

	vec->data = new;
	vec->nmemb = nmemb;
	vec->offset = offset;
	return true;
}

static bool vec_grow_right(vec_t *vec)
{
	if (vec->nused < (vec->nmemb - vec->offset)) {
		return true;
	}

	return vec_grow(vec);

}

static bool vec_grow_left(vec_t *vec)
{
	if (vec->offset > 0) {
		return true;
	}

	return vec_grow(vec);
}
bool vec_push(vec_t *vec, void *val)
{
	if (!vec_grow_right(vec)) return false;

	void *newelem = vec->cbs.vcpy(val);

	if (newelem == NULL) {
		return false;
	}

	*vec_elem(vec, vec->nused) = newelem;
	vec->nused += 1;

	return true;
}

bool vec_push_front(vec_t *vec, void *val)
{
	if (!vec_grow_left(vec)) {
		return false;
	}

	void *newelem = vec->cbs.vcpy(val);

	if (newelem == NULL) {
		return false;
	}

	vec->offset -= 1;
	*vec_elem(vec, 0) = newelem;
	vec->nused += 1;

	return true;
}

bool vec_remove(vec_t *vec, size_t i)
{
	if (vec == NULL || i >= vec->nused) {
		return false;
	}

	vec->cbs.vfree(*vec_elem(vec, i));

	vec->nused -= 1;

	if (i == vec->nused) {
		return true;
	}

	memmove(vec_elem(vec, i), vec_elem(vec, i + 1), sizeof (size_t[vec->nused - i]));
	return true;
}

void *vec_pop(vec_t *vec)
{

	if (vec->nused > 0) {
		vec->nused -= 1;
		return *vec_elem(vec, vec->nused);
	} else {
		return NULL;
	}
}

void *vec_pop_front (vec_t *vec)
{
	if (vec->nused > 0) {
		void *ret = vec_get(vec, 0);
		vec->offset += 1;
		vec->nused -= 1;

		if (vec->nused == 0) {
			vec->offset = vec->nmemb / 2;
		}
		return ret;
	} else {
		return NULL;
	}
}

void *vec_peek(vec_t *vec)
{
	if (vec->nused > 0) {
		return vec_get(vec, vec->nused - 1);
	} else {
		return NULL;
	}
}
size_t vec_len(vec_t *vec)
{
	return vec->nused;
}

bool vec_set(vec_t *vec, size_t i, void *newval)
{
	if (i > vec->nused) {
		return false;
	} else if (i == vec->nused) {
		return vec_push(vec, newval);
	}

	void **elem = vec_elem(vec, i);
	void *newelem = vec->cbs.vcpy(newval);

	vec->cbs.vfree(*elem);
	*elem = newelem;
	return true;
}

void *vec_get(vec_t *vec, size_t i)
{
	if (i >= vec->nused) {
		return NULL;
	} else {
		return *(vec_elem(vec, i));
	}
}

void vec_qsort(vec_t *vec, int (*compar)(const void *, const void *))
{
	if (!vec) {
		return;
	}

	qsort(vec_elem(vec, 0), vec->nused, sizeof (void *), compar);
}

static void *lfind (const void *key, const void *base, size_t *nmemb, size_t size,
       __compar_fn_t compar)
{
	const void *result = base;
	size_t cnt = 0;

	while (cnt < *nmemb && (*compar) (key, result) != 0)
	{
		result += size;
		++cnt;
	}

	return cnt < *nmemb ? (void *) result : NULL;
}

void *vec_lfind(vec_t *vec, const void *key, int (*compar)(const void *, const void *))
{
	return lfind(key, vec_elem(vec, 0), &vec->nused, sizeof (void *), compar);
}

void *vec_bsearch(vec_t *vec, const void *key, int (*compar)(const void *, const void *))
{
	return bsearch(key, vec_elem(vec, 0), vec->nused, sizeof (void *), compar);
}

size_t vec_collect(vec_t *vec, void ***save)
{
	if (save != NULL) {
		if (vec->nused < vec->nmemb) {
			*save = reallocarray(NULL, vec->nused, sizeof(void *));
			memcpy(*save, vec_elem(vec, 0), sizeof (size_t[vec->nused]));
			free(vec->data);
		} else {
			*save = vec->data;
		}
	}
	size_t ret = vec_len(vec);
	free(vec);

	return ret;
}