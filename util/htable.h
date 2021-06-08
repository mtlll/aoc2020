#ifndef HTABLE_H
#define HTABLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct htable;
typedef struct htable htable_t;

struct ht_enum;
typedef struct ht_enum ht_enum_t;

typedef size_t (*ht_hash)(const void *in, size_t seed);

typedef void *(*ht_kcpy)(void *in);

typedef bool (*ht_kcmp)(const void *a, const void *b);

typedef void (*ht_kfree)(void *in);

typedef void *(*ht_vcpy)(void *in);

typedef void (*ht_vfree)(void *in);

typedef struct {
    ht_kcpy kcpy;
    ht_kfree kfree;
    ht_vcpy vcpy;
    ht_vfree vfree;
} ht_callbacks;

htable_t *ht_create(ht_hash hfunc, ht_kcmp kcmp, ht_callbacks *cbs);

void ht_destroy(htable_t *ht);

size_t ht_size(htable_t *ht);
size_t ht_seed(htable_t *ht);
void ht_set_seed(htable_t *ht, size_t seed);
void *ht_insert(htable_t *ht, void *key, void *val);

void ht_remove(htable_t *ht, void *key);

bool ht_get(htable_t *ht, void *key, void **val);
bool ht_get_key(htable_t *ht, void *key, void **savekey, void **val);

ht_enum_t *ht_enum_create(htable_t *ht);
void ht_enum_reset(ht_enum_t *he);
bool ht_enum_next(ht_enum_t *he, void **key, void **val);
void ht_enum_destroy(ht_enum_t *he);

size_t fnv1a_64(const void *in, size_t len, size_t seed);

size_t hash_str(const void *in, size_t seed);

bool cmpstr(const void *a, const void *b);
size_t u64hash(const void *in, size_t seed);

bool u64cmp(const void *a, const void *b);

void *u64cpy(void *in);

#endif//HTABLE_H