#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "htable.h"

struct ht_bucket {
    void *key;
    void *val;
    struct ht_bucket *next;
};

typedef struct ht_bucket ht_bucket_t;

struct htable {
    ht_hash hfunc;
    ht_kcmp kcmp;
    ht_callbacks cbs;
    ht_bucket_t *buckets;
    size_t nbuckets;
    size_t nused;
    size_t seed;
};

struct ht_enum {
    htable_t *ht;
    ht_bucket_t *cur;
    size_t idx;
};

static const size_t BUCKET_START = 128;

static void *ht_passthrough_cpy(void *v)
{
	return v;
}

static void ht_passthrough_destroy(void *v)
{
	return;
}

static size_t ht_bucket_idx(htable_t *ht, void *key)
{
	return ht->hfunc(key, ht->seed) % ht->nbuckets;
}

static void *ht_add_to_bucket(htable_t *ht, void *key, void *val, bool isrehash)
{
	ht_bucket_t *cur;
	ht_bucket_t *last;
	size_t idx;

	idx = ht_bucket_idx(ht, key);
	if (ht->buckets[idx].key == NULL) {
		if (!isrehash) {
			key = ht->cbs.kcpy(key);
			if (val != NULL) {
				val = ht->cbs.vcpy(val);
			}
		}
		ht->buckets[idx].key = key;
		ht->buckets[idx].val = val;

		if (!isrehash) {
			ht->nused += 1;
		}
	} else {
		cur = last = ht->buckets + idx;
		do {
			if (ht->kcmp(key, cur->key)) {
				if (cur->val != NULL) {
					ht->cbs.vfree(cur->val);
				}
				if (!isrehash && val != NULL) {
					val = ht->cbs.vcpy(val);
				}
				cur->val = val;
				last = NULL;
				break;
			}
			last = cur;
			cur = cur->next;
		} while (cur != NULL);

		if (last != NULL) {
			cur = calloc(1, sizeof(ht_bucket_t));
			if (!isrehash) {
				key = ht->cbs.kcpy(key);
				if (val != NULL) {
					val = ht->cbs.vcpy(val);
				}
			}

			cur->key = key;
			cur->val = val;
			last->next = cur;
			if (!isrehash) {
				ht->nused += 1;
			}
		}
	}

	return val;
}

static void ht_rehash(htable_t *ht)
{
	ht_bucket_t *buckets;
	ht_bucket_t *cur;
	ht_bucket_t *next;
	size_t nbuckets, i;

	if (ht->nused + 1 < (size_t) (ht->nbuckets * 0.75) ||
	    ht->nbuckets >= 1 << 31) {
		return;
	}

	nbuckets = ht->nbuckets;
	buckets = ht->buckets;
	ht->nbuckets <<= 2;
	ht->buckets = calloc(ht->nbuckets, sizeof(ht_bucket_t));

	for (i = 0; i < nbuckets; i += 1) {
		if (buckets[i].key == NULL) {
			continue;
		}

		ht_add_to_bucket(ht, buckets[i].key, buckets[i].val, true);

		if (buckets[i].next != NULL) {
			cur = buckets[i].next;
			do {
				ht_add_to_bucket(ht, cur->key, cur->val, true);
				next = cur->next;
				free(cur);
				cur = next;
			} while (cur != NULL);
		}
	}

	free(buckets);
}

htable_t *ht_create(ht_hash hfunc, ht_kcmp kcmp, ht_callbacks *cbs)
{
	htable_t *ht;

	if (hfunc == NULL || kcmp == NULL) {
		return NULL;
	}

	ht = calloc(1, sizeof(htable_t));
	ht->hfunc = hfunc;
	ht->kcmp = kcmp;

	ht->cbs.kcpy = ht_passthrough_cpy;
	ht->cbs.kfree = ht_passthrough_destroy;
	ht->cbs.vcpy = ht_passthrough_cpy;
	ht->cbs.vfree = ht_passthrough_destroy;

	if (cbs != NULL) {
		if (cbs->kcpy != NULL) ht->cbs.kcpy = cbs->kcpy;
		if (cbs->kfree != NULL) ht->cbs.kfree = cbs->kfree;
		if (cbs->vcpy != NULL) ht->cbs.vcpy = cbs->vcpy;
		if (cbs->vfree != NULL) ht->cbs.vfree = cbs->vfree;
	}

	ht->nbuckets = BUCKET_START;
	ht->buckets = calloc(BUCKET_START, sizeof(ht_bucket_t));

	ht->seed = (size_t) time(NULL);
	ht->seed ^= ((size_t) ht_create << 16) | (size_t) ht;
	ht->seed ^= ((size_t) &ht->seed);

	return ht;
}

void ht_destroy(htable_t *ht)
{
	ht_bucket_t *next;
	ht_bucket_t *cur;
	size_t i;

	if (ht == NULL) {
		return;
	}

	for (i = 0; i < ht->nbuckets; i += 1) {

		next = ht->buckets[i].next;
		if (ht->buckets[i].key == NULL) {
			continue;
		}

		ht->cbs.kfree(ht->buckets[i].key);
		ht->cbs.vfree(ht->buckets[i].val);

		while (next != NULL) {
			cur = next;
			ht->cbs.kfree(cur->key);
			ht->cbs.vfree(cur->val);
			next = cur->next;
			free(cur);
		}
	}

	free(ht->buckets);
	free(ht);
}


size_t ht_seed(htable_t *ht)
{
	return ht->seed;
}
void ht_set_seed(htable_t *ht, size_t seed)
{
	ht->seed = seed;
}
size_t ht_size(htable_t *ht)
{
	return ht->nused;
}
void *ht_insert(htable_t *ht, void *key, void *val)
{
	if (ht == NULL || key == NULL) {
		return NULL;
	}

	ht_rehash(ht);
	return ht_add_to_bucket(ht, key, val, false);
}

void ht_remove(htable_t *ht, void *key)
{
	ht_bucket_t *cur;
	ht_bucket_t *last;
	size_t idx;

	if (ht == NULL || key == NULL) {
		return;
	}

	idx = ht_bucket_idx(ht, key);
	if (ht->buckets[idx].key == NULL) {
		return;
	}

	if (ht->kcmp(ht->buckets[idx].key, key)) {
		ht->cbs.kfree(ht->buckets[idx].key);
		ht->cbs.vfree(ht->buckets[idx].val);
		ht->buckets[idx].key = NULL;

		cur = ht->buckets[idx].next;
		if (cur != NULL) {
			ht->buckets[idx].key = cur->key;
			ht->buckets[idx].val = cur->val;
			ht->buckets[idx].next = cur->next;
		}

		ht->nused -= 1;
		return;
	}

	last = ht->buckets + idx;
	cur = last->next;

	while (cur != NULL) {
		if (ht->kcmp(key, cur->key)) {
			last->next = cur->next;
			ht->cbs.kfree(cur->key);
			ht->cbs.vfree(cur->val);
			free(cur);
			ht->nused -= 1;
			break;
		}
		last = cur;
		cur = cur->next;
	}
}

bool ht_get_key(htable_t *ht, void *key, void **savekey, void **val)
{
	ht_bucket_t *cur;
	size_t idx;

	if (ht == NULL || key == NULL) {
		return false;
	}

	idx = ht_bucket_idx(ht, key);
	if (ht->buckets[idx].key == NULL) {
		return false;
	}

	cur = ht->buckets + idx;
	while (cur != NULL) {
		if (ht->kcmp(key, cur->key)) {
			if (val != NULL) {
				*val = cur->val;
			}
			if (savekey != NULL) {
				*savekey = cur->key;
			}
			return true;
		}
		cur = cur->next;
	}

	return false;
}

bool ht_get(htable_t *ht, void *key, void **val)
{
	return ht_get_key(ht, key, NULL, val);
}
ht_enum_t *ht_enum_create(htable_t *ht)
{
	ht_enum_t *he;

	if (ht == NULL) {
		return NULL;
	}

	he = calloc(1, sizeof(*he));
	he->ht = ht;

	return he;
}

void ht_enum_reset(ht_enum_t *he)
{
	he->cur = NULL;
	he->idx = 0;
}
bool ht_enum_next(ht_enum_t *he, void **key, void **val)
{
	void *myval;
	void *mykey;
	htable_t *ht = he->ht;

	if (he == NULL) {
		return false;
	}

	if (key == NULL) {
		key = &mykey;
	}
	if (val == NULL) {
		val = &myval;
	}

	if (he->cur == NULL) {
		while (he->idx < ht->nbuckets &&
		       ht->buckets[he->idx].key == NULL) {
			he->idx += 1;
		}

		if (he->idx >= ht->nbuckets) {
			return false;
		}

		he->cur = ht->buckets + he->idx;
		he->idx += 1;
	}

	*key = he->cur->key;
	*val = he->cur->val;
	he->cur = he->cur->next;

	return true;
}

void ht_enum_destroy(ht_enum_t *he)
{
	free(he);
}

size_t fnv1a_64(const void *in, size_t len, size_t seed)
{
	size_t h = seed;
	size_t c, i;
	register size_t p = (1ul << 40) + (1 << 8) + 0xb3;
	for (i = 0; i < len; i += 1) {
		c = ((const unsigned char *) in)[i];
		h ^= c;
		h *= p;
	}

	return h;
}

size_t hash_str(const void *in, size_t seed)
{
	return fnv1a_64(in, strlen(in), seed);
}

bool cmpstr(const void *a, const void *b)
{
	return strcmp(a, b) == 0 ? true : false;
}
size_t u64hash(const void *in, size_t seed)
{
	return fnv1a_64(in, 8, seed);
}

bool u64cmp(const void *a, const void *b)
{
	return *((uint64_t *) a) == *((uint64_t *) b) ? true : false;
}

void *u64cpy(void *in) {
	void *ret = malloc(sizeof (uint64_t));
	memcpy(ret, in, sizeof (uint64_t));
	return ret;
}