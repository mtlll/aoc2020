#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/htable.h"
#include "util/io.h"
#include "util/vec.h"
#include "day.h"

typedef struct {
    size_t nlines;
    char **lines;
} input;

static void freeinput(void *in)
{
	input *inp = in;

	for (size_t i = 0; i < inp->nlines; i += 1) {
		free(inp->lines[i]);
	}

	free(inp);
}
static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day6/", filename, "r");
	char *s = fslurp(f, "\r");
	fclose(f);

	input *res = malloc(sizeof (input));
	vec_callbacks cbs = {strdup, NULL};

	vec_t *lines = vec_create(&cbs);
	char *w = s;
	char *tok;

	while ((tok = strsep(&w, "\n"))) {
		vec_push(lines, tok);
	}

	res->nlines = vec_collect(lines, (void ***)&res->lines);

	free(s);

	return (void *)res;
}

static result *part1(void *data)
{
	input *inp = data;
	size_t nlines = inp->nlines;
	char *seen = calloc(28, sizeof(char));
	uint64_t count = 0;
	uint64_t res = 0;

	char *tok;

	for (size_t i = 0; i < nlines; i += 1) {
		tok = inp->lines[i];

		if (*tok == '\0') {
			res += count;
			bzero(seen, count);
			count = 0;
			continue;
		}

		for (tok += strspn(tok, seen); *tok != '\0'; tok += strspn(tok, seen)) {
			seen[count++] = *tok;
		}
	}

	free(seen);
	return_result(res);
}

static void swap(char *a, char *b)
{
	if (a != b) {
		char tmp = *a;
		*a = *b;
		*b = tmp;
	}
}

static result *part2(void *data)
{
	input *inp = data;
	size_t nlines = inp->nlines;

	char *seen = NULL;
	uint64_t res = 0;
	char *tok;

	for (size_t i = 0; i < nlines; i += 1) {
		tok = inp->lines[i];
		if (*tok == '\0') {
			res += strlen(seen);
			seen = NULL;
			continue;
		} else if (seen == NULL) {
			seen = tok;
			continue;
		}

		for (char *s = seen; *s; s += 1) {
			if (!strchr(tok, *s)) {
				swap(seen++, s);
			}
		}
	}

	return_result(res);
}

day day6 = {
	getinput,
	part1,
	part2,
	freeinput
};

