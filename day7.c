#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/htable.h"
#include "util/io.h"
#include "util/vec.h"
#include "day.h"



typedef struct {
    char *type;
    unsigned int n;
} bagspec;

typedef struct {
    char *type;
    bagspec **bags;
    size_t nbags;
} bag;

void bagfree(void *in)
{
	bag *b = in;
	for (size_t i = 0; i < b->nbags; i += 1) {
		free(b->bags[i]->type);
		free(b->bags[i]);
	}
	free(b->bags);
	free(b->type);
	free(in);
}

static bool stringcmp(const void *a, const void *b)
{
	return strcmp(a, b) == 0;
}

static bag *parse_bag(char *line)
{
	bag *b = calloc(1, sizeof(bag));

	char *s = strstr(line, " bags contain ");
	*s = '\0';
	b->type = strdup(line);

	char *w = s + 14;

	if (!isdigit(w[0])) {
		return b;
	}

	vec_t *bags = vec_create(NULL);

	while (w && *w) {
		bagspec *curbag = calloc(1, sizeof(bagspec));
		curbag->n = atoi(strsep(&w, " "));
		s = strstr(w, " bag");
		*s = '\0';
		s += 4;
		curbag->type = strdup(w);
		vec_push(bags, curbag);
		w = s + strspn(s, "s,. ");
	}

	b->nbags = vec_collect(bags, &b->bags);

	return b;
}

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day7/", filename, "r");
	char **lines;
	size_t nlines = get_lines(f, &lines);
	fclose(f);
	ht_callbacks cbs = {
		(ht_kcpy) strdup,
		free,
		NULL,
		bagfree};

	htable_t *ht = ht_create(hash_str, stringcmp, &cbs);
	bag *b;

	for (size_t i = 0; i < nlines; i += 1) {
		b = parse_bag(lines[i]);
		ht_insert(ht, b->type, b);
		free(lines[i]);
	}

	free(lines);
	return (void *)ht;
}

static bool find(char *needle, bag *substack, htable_t *haystack)
{
	for (size_t i = 0; i < substack->nbags; i += 1) {
		if (stringcmp(needle, substack->bags[i]->type)) {
			return true;
		}
	}

	bag *newbag;

	for (size_t i = 0; i < substack->nbags; i += 1) {
		ht_get(haystack, substack->bags[i]->type, (void **)&newbag);
		if (find(needle, newbag, haystack)) {
			return true;
		}
	}

	return false;
}

static result *part1(void *data)
{
	htable_t *bags = data;
	char needle[] = "shiny gold";
	unsigned long count = 0;

	ht_enum_t *he = ht_enum_create(bags);

	char *curtype;
	bag *curb;

	while (ht_enum_next(he, (void **)&curtype, (void **)&curb)) {
		if (find(needle, curb, bags)) {
			count += 1;
		}
	}

	ht_enum_destroy(he);
	return_result(count);
}

static unsigned long sum_subbags(char *needle, htable_t *bags)
{
	unsigned long res = 0;

	bag *b;
	ht_get(bags, needle, (void **)&b);

	for (size_t i = 0; i < b->nbags; i += 1) {
		res += b->bags[i]->n +
		       b->bags[i]->n * sum_subbags(b->bags[i]->type, bags);
	}

	return res;
}

static result *part2(void *data)
{
	return_result(sum_subbags("shiny gold", (htable_t *)data));
}

day day7 = {
	getinput,
	part1,
	part2,
	(freedata)ht_destroy
};