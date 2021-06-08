#include <stdio.h>
#include <stdlib.h>

#include "util/io.h"
#include "day.h"

static int uint64cmp(const void *a, const void *b)
{
	return *((uint64_t *) a) - *((uint64_t *) b);
}

typedef struct {
    size_t nadapters;
    uint64_t adapters[];
} input;

void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day10/", filename, "r");
	char **lines;
	size_t nlines = get_lines(f, &lines);
	fclose(f);

	input *res = malloc(sizeof(input) + sizeof (uint64_t[nlines]));

	for (size_t i = 0; i < nlines; i += 1) {
		res->adapters[i] = strtoul(lines[i], NULL, 10);
		free(lines[i]);
	}

	qsort(res->adapters, nlines, sizeof(uint64_t), &uint64cmp);
	free(lines);
	res->nadapters = nlines;

	return (void *)res;
}

result *part1(void *data)
{
	input *inp = data;
	size_t nad = inp->nadapters;
	uint64_t *ad = inp->adapters;
	uint64_t diff1 = 0;
	uint64_t diff3 = 1;

	for (size_t i = nad - 1; i > 0; i -= 1) {
		if (ad[i] - ad[i - 1] == 3) {
			diff3 += 1;
		} else {
			diff1 += 1;
		}
	}

	if (ad[0] == 3) {
		diff3 += 1;
	} else {
		diff1 += 1;
	}

	return_result(diff3 * diff1);
}


result *part2(void *data)
{
	input *inp = data;
	size_t nad = inp->nadapters;
	uint64_t *ad = inp->adapters;

	size_t i, j;
	uint64_t res;
	uint64_t *counts = calloc(ad[nad - 1] + 1, sizeof(uint64_t));

	for (i = 0; ad[i] <= 3; i += 1) {
		counts[ad[i]] += 1;
	}

	for (i = 0; i < nad - 1; i += 1) {
		for (j = i + 1; ad[j] - ad[i] <= 3; j += 1) {
			counts[ad[j]] += counts[ad[i]];
		}
	}

	res = counts[ad[i]];
	free(counts);
	return_result(res);
}

day day10 = {
	getinput,
	part1,
	part2,
	free
};