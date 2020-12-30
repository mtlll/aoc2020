#ifndef HTABLE_H
#define HTABLE_H

#include <stddef.h>
#include <stdbool.h>

struct htable;
typedef struct htable htable_t;

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
void ht_destrpy(htable_t *ht);

void ht_insert(htable_t *ht, void *key, void *val);
void ht_remove(htable_t *ht, void *key);

bool ht_get(htable_t *ht, void *key, void **val);

#endif //HTABLE_H
