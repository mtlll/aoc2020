#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/io.h"
#include "day.h"

typedef struct {
    int min;
    int max;
    char c;
    char *pw;
} pwspec;

typedef struct input {
    size_t nentries;
    pwspec entries[];
} input;

static void freeinput(void *in)
{
	input *inp = in;
	for (size_t i = 0; i < inp->nentries; i += 1) {
		free(inp->entries[i].pw);
	}

	free(in);
}
static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day2", filename, "r");
	char **lines;
	size_t nlines = get_lines(f, &lines);
	fclose(f);


	input *r = malloc(sizeof (input) + sizeof (pwspec[nlines]));
	pwspec *p;
	char *w, *tok;
	size_t i;

	for (i = 0; i < nlines; i += 1) {
		p = r->entries + i;
		w = lines[i];
		tok = strsep(&w, " ");
		sscanf(tok, "%d-%d", &p->min, &p->max);
		tok = strsep(&w, " ");
		p->c = tok[0];
		p->pw = strdup(w);
		free(lines[i]);
	}

	free(lines);

	r->nentries = nlines;
	return (void *)r;
}

static result *part1(void *data)
{

	input *inp = data;
	size_t nentries = inp->nentries;
	pwspec *entries = inp->entries;
	pwspec *p;


	int charcount;
	unsigned long validcount = 0;
	size_t i, j;

	for (i = 0; i < nentries; i += 1) {
		p = entries + i;
		for (j = charcount = 0; p->pw[j]; j += 1) {
			if (p->pw[j] == p->c) {
				charcount += 1;
			}
		}

		if (charcount >= p->min && charcount <= p->max) {
			validcount += 1;
		}
	}

	return_result(validcount);
}

static result *part2(void *data)
{
	input *inp = data;
	size_t nentries = inp->nentries;
	pwspec *entries = inp->entries;
	pwspec *p;

	unsigned long validcount = 0;
	size_t i, mn, mx;

	for (i = 0; i < nentries; i += 1) {
		p = entries + i;
		mn = p->min - 1;
		mx = p->max - 1;

		/* logical xor */
		if ((p->pw[mn] != p->c) != (p->pw[mx] != p->c)) {
			validcount += 1;
		}
	}


	return_result(validcount);
}

day day2 = {
	getinput,
	part1,
	part2,
	freeinput
};