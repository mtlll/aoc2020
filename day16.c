#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "day.h"
#include "util/io.h"
#include "util/vec.h"
#include "util/htable.h"

typedef struct {
    uint64_t min, max, midmax, midmin;
    size_t offset;
    bool solved;
} rule;

typedef struct {
    size_t nfields;
    htable_t *rules;
    vec_t *tickets;
} input;

static void freeinput(void *in)
{
	input *inp = in;

	ht_destroy(inp->rules);
	vec_destroy(inp->tickets);
	free(inp);
}
static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day16/", filename, "r");

	char *s = fslurp(f, "\r");

	fclose(f);

	input *ret = calloc(1, sizeof (input));

	ht_callbacks cbs = {
		(ht_kcpy)strdup,
		free,
		NULL,
		free,
	};

	htable_t *ht = ht_create(hash_str, cmpstr, &cbs);

	char *w = s;
	char *tok;
	rule *r;
	char *fname;

	while ((tok = strsep(&w, "\n")) && *tok) {
		r = calloc(1, sizeof (rule));
		fname = strsep(&tok, ":");
		sscanf(tok, " %lu-%lu or %lu-%lu",
	 		&r->min, &r->midmax, &r->midmin, &r->max);
		ht_insert(ht, fname, r);
		ret->nfields += 1;
	}
	ret->rules = ht;

	vec_callbacks vcbs = {NULL, free};
	vec_t *vec = vec_create(&vcbs);
	uint64_t *t;

	while ((tok = strsep(&w, "\n"))) {
		if (!isdigit(*tok)) {
			continue;
		}
		t = calloc(ret->nfields, sizeof (uint64_t));
		for (size_t i = 0; i < ret->nfields; i += 1) {
			t[i] = strtoul(tok, &tok, 10);
			tok += 1;
		}

		vec_push(vec, t);
	}

	ret->tickets = vec;

	free(s);

	return (void *)ret;
}

static bool check_field(uint64_t fi, const rule *r)
{
	if (fi >= r->min && fi <= r->midmax) {
		return true;
	} else if (fi >= r->midmin && fi <= r->max) {
		return true;
	} else {
		return false;
	}
}

static bool
check_offset(size_t offset, vec_t *tickets, rule *r)
{
	uint64_t *ticket;
	size_t ntickets = vec_len(tickets);

	for (size_t i = 0; i < ntickets; i += 1) {
		ticket = vec_get(tickets, i);
		if (ticket == NULL) {
			continue;
		}
		if (!check_field(ticket[offset], r)) {
			return false;
		}
	}

	return true;
}

static size_t
find_offset(input *inp, rule *r)
{
	for (size_t i = 0; i < inp->nfields; i += 1) {
		if (check_offset(i, inp->tickets, r)) {
			return i;
		}
	}

	return 0;
}
static bool field_valid(uint64_t fi, htable_t *rules)
{
	rule *r;
	ht_enum_t *he = ht_enum_create(rules);
	bool ret = false;

	while (ht_enum_next(he, NULL, (void **)&r)) {
		if (check_field(fi, r)) {
			ret = true;
			break;
		}
	}

	ht_enum_destroy(he);
	return ret;
}

static result *part1(void *data)
{
	input *inp = data;

	uint64_t error_rate = 0;
	uint64_t fi;
	uint64_t *ticket;
	size_t ntickets = vec_len(inp->tickets);

	for (size_t i = 0; i < ntickets; i += 1) {
		ticket = vec_get(inp->tickets, i);
		for (size_t j = 0; j < inp->nfields; j += 1) {
			fi = ticket[j];
			if (!field_valid(fi, inp->rules)) {
				error_rate += fi;
				vec_set(inp->tickets, i, NULL);
				break;
			}
		}
	}

	return_result(error_rate);
}

static result *part2(void *data)
{
	input *inp = data;
	vec_t *tickets = inp->tickets;
	htable_t *rules = inp->rules;
	size_t nfields = inp->nfields;
	uint64_t *myticket = vec_get(tickets, 0);

	bool *taken = calloc(nfields, sizeof (bool));
	size_t nsolved = 0;
	uint64_t res = 1;

	ht_enum_t *he = ht_enum_create(inp->rules);
	rule *r;
	char *fname;

	while (nsolved < inp->nfields) {
		while (ht_enum_next(he, &fname, &r)) {
			register bool *solved = &r->solved;
			register size_t *offset = &r->offset;

			if (*solved) {
				continue;
			}

			for (size_t i = *offset; i < nfields; i += 1) {
				if (taken[i]) {
					continue;
				}

				if (check_offset(i, tickets, r)) {
					if (!(*solved)) {
						*offset = i;
						*solved = true;
					} else {
						*solved = false;
						break;
					}
				}
			}

			if (*solved) {
				taken[r->offset] = true;
				nsolved += 1;

				if (strncmp(fname, "departure", 9) == 0) {
					res *= myticket[*offset];
				}
			}
		}
		ht_enum_reset(he);
	}

	ht_enum_destroy(he);
	return_result(res);
}

day day16 = {
	getinput,
	part1,
	part2,
	freeinput,
};