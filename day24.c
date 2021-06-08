#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "day.h"
#include "flags.h"
#include "util/io.h"
#include "util/vec.h"
#include "util/htable.h"
#include "util/list.h"

typedef struct {
    vec_t *tiles;
    htable_t *floor;
    list_t blacks;
    uint64_t blackcount;
} input;

static void freeinput (input *inp)
{
	ht_destroy(inp->floor);
	vec_destroy(inp->tiles);
	free(inp);
}

static void *getinput(char *filename)
{

	FILE *f = fopen_in_path("../inputs/day24/", filename, "r");
	char *s = fslurp(f, "\r");
	fclose(f);

	vec_callbacks cbs = {strdup, free};

	input *ret = malloc(sizeof (input));
	init_list(&ret->blacks);
	ret->tiles = vec_create(&cbs);

	char *w = s;
	char *tok;

	while ((tok = strsep(&w, "\n")) && *tok) {
		vec_push(ret->tiles, tok);
	}

	free(s);
	return (void *)ret;
}

typedef enum {
    E = 0,
    SE = 1,
    SW = 2,
    W = 3,
    NW = 4,
    NE = 5,
} dir;

typedef struct {
	union {
	    struct {
		int32_t q;
		int32_t r;
	    };
	    uint64_t data;
	};
	list_t blacks;
} htile;

static htile *htilecpy(htile *t)
{
	htile *ret = malloc(sizeof (htile));

	ret->data = t->data;
	init_list(&ret->blacks);
	return ret;
}

static bool htilecmp(htile *t1, htile *t2)
{
	return t1->q == t2->q && t1->r == t2->r;
}

void reset_tile(htile *t)
{
	t->q = t->r = 0;
}
void move(htile *t, dir d)
{
	switch(d) {
		case E:
			t->q += 1;
			break;
		case SE:
			t->r += 1;
			break;
		case SW:
			t->q -= 1;
			t->r += 1;
			break;
		case W:
			t->q -= 1;
			break;
		case NW:
			t->r -= 1;
			break;
		case NE:
			t->q += 1;
			t->r -= 1;
			break;
	}

	assert(t->q + t->r + (-t->q - t->r) == 0);
}

static result *part1(input *inp)
{
	vec_t *tiles = inp->tiles;
	htile *t = calloc(1, sizeof (htile));
	htile *newt;
	char *s;
	ht_callbacks cbs = {NULL, free, u64cpy, free};
	uint64_t black = true;
	uint64_t *colour;

	htable_t *flipped = inp->floor = ht_create(u64hash, u64cmp, &cbs);

	if (flags & VERBOSE) printf("\n");
	for (size_t i = 0; i < vec_len(tiles); i += 1) {
		s = vec_get(tiles, i);
		if (flags & VERBOSE) printf("%s\t\t", s);
		while (*s) {
			if (*s == 'e') {
				move(t, E);
			} else if (*s == 'w') {
				move(t, W);
			} else if (*s == 'n') {
				s += 1;
				if (*s == 'e') {
					move(t, NE);
				} else if (*s == 'w') {
					move(t, NW);
				}
			} else if (*s == 's') {
				s += 1;
				if (*s == 'e') {
					move(t, SE);
				} else if (*s == 'w') {
					move(t, SW);
				}
			}
			s += 1;
		}
		if (flags & VERBOSE) printf("(%d, %d) = ", t->q, t->r);
		if (ht_get(flipped, t, &colour)) {
			*colour ^= black;

		} else {
			ht_insert(flipped, htilecpy(t), &black);
			if (flags & VERBOSE) printf("black\n");
		}

		reset_tile(t);
	}
	free(t);
	inp->blackcount = 0;
	ht_enum_t *he = ht_enum_create(flipped);
	while (ht_enum_next(he, &t, &colour)) {
		if (*colour) {
			inp->blackcount += 1;
			list_add(&t->blacks, &inp->blacks);
		}
	}
	ht_enum_destroy(he);

	return_result(inp->blackcount);
}

static void print_tile(htile *t)
{
	printf("(%d, %d)", t->q, t->r);
}
static void nb_tile(htile *t, htile *nb, dir d)
{
	*((uint64_t *) nb) = *((uint64_t *) t);
	move(nb, d);
}

static uint8_t count_neighbours(htile *t, htable_t *floor)
{
	htile nb;
	uint8_t ret = 0;
	uint64_t *colour;

	for (dir d = E; d <= NE; d += 1) {
		nb_tile(t, &nb, d);
		if (ht_get(floor, &nb, &colour) && *colour) {
			ret += 1;
		}
	}

	return ret;
}
static void check_neighbours (htile *t, htable_t *floor, list_t *new_blacks)
{
	uint64_t black = true;
	uint64_t white = false;
	uint64_t *colour;
	uint8_t cnt;
	htile nb;
	htile *actual;

	for (dir d = E; d <= NE; d += 1) {
		nb_tile(t, &nb, d);
		if (!ht_get_key(floor, &nb, &actual, &colour)) {
			actual = htilecpy(&nb);
			colour = ht_insert(floor, actual, &white);
		}

		if (list_empty(&actual->blacks)) {
			cnt = count_neighbours(actual, floor);
			if (cnt == 2) {
				list_add(&actual->blacks, new_blacks);
			}
		}
	}
}
static result *part2(input *inp)
{
	uint64_t white = 0;
	uint64_t black = 1;
	list_t *blacks = &inp->blacks;
	list_t new_blacks, new_whites;
	init_list(&new_blacks);
	init_list(&new_whites);
	htable_t *floor = inp->floor;
	htile *cur, *next;
	uint8_t cnt;

	for (size_t i = 0; i < 100; i += 1) {
		list_for_each_entry_safe(cur, next, blacks, blacks) {
			cnt = count_neighbours(cur, floor);
			if (cnt == 0 || cnt > 2) {
				list_del(&cur->blacks);
				list_add(&cur->blacks, &new_whites);
				inp->blackcount -= 1;
			}

			check_neighbours(cur, floor, &new_blacks);
		}

		list_for_each_entry(cur, &new_blacks, blacks) {
			inp->blackcount += 1;
			ht_insert(floor, cur, &black);
		}

		list_splice_init(&new_blacks, blacks);

		list_for_each_entry_safe(cur, next, &new_whites, blacks) {
			ht_insert(floor, cur, &white);
			list_del(&cur->blacks);
		}
	}

	return_result(inp->blackcount);
}

day day24 = {
	getinput,
	part1,
	part2,
	freeinput,
};