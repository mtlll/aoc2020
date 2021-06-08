#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/io.h"
#include "day.h"

struct input {
    size_t nentries;
    int entries[];
};

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day1/", filename, "r");

	char **lines;
	size_t nlines = get_lines(f, &lines);
	fclose(f);

	struct input *res = malloc(sizeof (struct input) + sizeof (int[nlines]));
	for (size_t i = 0; i < nlines; i += 1) {
		res->entries[i] = atoi(lines[i]);
	}

	res->nentries = nlines;
	return (void *)res;
}

static result *part1(void *data)
{
	struct input *inp = data;
	size_t nentries = inp->nentries;
	int *entries = inp->entries;
	unsigned long res = 0;

	int i, j;
	for (i = 0; i < nentries; ++i) {
		for (j = i; j < nentries; ++j) {
			if (entries[i] + entries[j] == 2020) {
				res = entries[i] * entries[j];
				goto ret;
			}
		}
	}

	ret:
	return_result(res);
}

static result *part2(void *data)
{
	struct input *inp = data;

	size_t nentries = inp->nentries;
	int *e = inp->entries;
	unsigned long res = 0;
	size_t i, j, k;

	for (i = 0; i < nentries; ++i) {
		if (e[i] > 2020) {
			continue;
		}
		for (j = i; j < nentries; ++j) {
			if (e[i] + e[j] > 2020) {
				continue;
			}
			for (k = j; k < nentries; ++k) {
				if (e[i] + e[j] + e[k] == 2020) {
					res =  e[i] * e[j] * e[k];
					goto ret;
				}
			}
		}
	}

	ret:
	return_result(res);
}

day day1 = {
	getinput,
	part1,
	part2,
	free
};