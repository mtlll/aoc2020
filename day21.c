#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "day.h"
#include "util/io.h"
#include "util/vec.h"
#include "util/htable.h"

typedef struct {
    vec_t *ingred;
    vec_t *allergs;
} dish;

dish *new_dish()
{
	dish *ret = calloc(0, sizeof (dish));

	vec_callbacks cbs = {strdup, free};

	ret->ingred = vec_create(&cbs);
	ret->allergs = vec_create(&cbs);

	return ret;
}

void free_dish(dish *d)
{
	vec_destroy(d->ingred);
	vec_destroy(d->allergs);
	free(d);
}

typedef struct {
    size_t ndishes;
    dish **dishes;
    htable_t *freq;
    htable_t *allergs;
    size_t unknown;
} input;

static void freeinput(void *in)
{
	input *inp = in;
	for (size_t i = 0; i < inp->ndishes; i += 1) {
		free_dish(inp->dishes[i]);
	}

	ht_destroy(inp->freq);
	ht_destroy(inp->allergs);
	free(in);
}

typedef enum {
    INGRED,
    ALLERGS,
} state;

int comparstr(const void *a, const void *b)
{
	return strcmp(*((char **)a), *((char **)b));
}

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day21/", filename, "r");

	char *s = fslurp(f, "\r");

	fclose(f);

	input *ret = calloc(1, sizeof (input));

	vec_t *dishes = vec_create(NULL);
	ht_callbacks fcbs = {NULL, NULL, u64cpy, free};
	ht_callbacks acbs = {NULL, NULL, vec_cpy, vec_destroy};
	ret->freq = ht_create(hash_str, cmpstr, &fcbs);
	ret->allergs = ht_create(hash_str, cmpstr, &acbs);

	char *w = s;
	char *tok, *tok2;
	dish *d;
	state state;
	uint64_t one = 1;
	uint64_t *count;

	while ((tok = strsep(&w, "\n")) && *tok) {
		d = new_dish();
		state = INGRED;
		while ((tok2 = strsep(&tok, " ")) && *tok2) {
			if (*tok2 == '(') {
				state = ALLERGS;
			} else if (state == INGRED) {
				vec_push(d->ingred, tok2);
				if (ht_get(ret->freq, tok2, &count)) {
					*count += 1;
				} else {
					ht_insert(ret->freq, tok2, &one);
				}
			} else {
				tok2 = strsep(&tok2, ",)");
				vec_push(d->allergs, tok2);
				if (!ht_get(ret->allergs, tok2, NULL)) {
					ht_insert(ret->allergs, tok2, d->ingred);
					if (vec_len(d->ingred) > 1) {
						ret->unknown += 1;
					}
				}
			}
		}

		vec_qsort(d->ingred, comparstr);
		vec_push(dishes, d);
	}

	ret->ndishes = vec_collect(dishes, (void ***) &ret->dishes);
	free(s);
	return (void *)ret;

}


static char *eliminate_suspects(dish *d, vec_t *known, vec_t *suspects)
{
	char *ret = NULL;
	char *suspect;
	for (size_t k = 0;
	     k < vec_len(suspects);) {
		suspect = vec_get(suspects, k);
		if (vec_lfind(known, &suspect, comparstr)) {
			vec_remove(suspects, k);
		} else if (!vec_bsearch(d->ingred, &suspect, comparstr)) {
			vec_remove(suspects, k);
		} else {
			k += 1;
		}
	}

	if (vec_len(suspects) == 1) {
		ret = vec_peek(suspects);
	}

	return ret;
}

static void check_allergens(input *inp, dish *d, vec_t *known)
{
	char *a;
	vec_t *suspects;

	for (size_t j = 0; j < vec_len(d->allergs); j += 1) {
		a = vec_get(d->allergs, j);
		if (ht_get(inp->allergs, a, &suspects)) {
		    if (vec_len(suspects) > 1) {
			    char *suspect = eliminate_suspects(
				    d, known, suspects);

			    if (suspect) {
				    ht_remove(inp->freq, suspect);
				    vec_push(known, suspect);
			    }
		    }

		}
	}
}

static uint64_t count_inert(input *inp)
{
	uint64_t res = 0;
	uint64_t *count;
	ht_enum_t *he = ht_enum_create(inp->freq);

	char *ing;
	while (ht_enum_next(he, &ing, &count)) {
		//printf("\n%s = %lu", ing, *count);
		res += *count;
	}

	ht_enum_destroy(he);
	return res;
}

static result *part1(void *data)
{
	input *inp = data;
	dish *d;
	uint64_t newcnt = 0;

	vec_t *known = vec_create(NULL);

	do {

		for (size_t i = 0; i < inp->ndishes; i += 1) {
			d = inp->dishes[i];

			check_allergens(inp, d, known);

		}
	} while (vec_len(known) < inp->unknown);

	uint64_t res = count_inert(inp);

	vec_destroy(known);

	return_result(res);
}

static result *part2(void *data)
{
	input *inp = data;

	vec_t *keys = vec_create(NULL);

	ht_enum_t *he = ht_enum_create(inp->allergs);

	char *key;

	while(ht_enum_next(he, &key, NULL)) {
		vec_push(keys, key);
	}

	vec_qsort(keys, comparstr);

	char *new, *old;
	vec_t *ing;
	ht_get(inp->allergs, vec_get(keys, 0), &ing);
	old = strdup(vec_get(ing, 0));

	for (size_t i = 1; i < vec_len(keys); i += 1) {
		ht_get(inp->allergs, vec_get(keys, i), &ing);
		asprintf(&new, "%s,%s", old, vec_get(ing, 0));
		free(old);
		old = new;
	}

	return_result(new);
}

day day21 = {
	getinput,
	part1,
	part2,
	freeinput,
};