#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "day.h"
#include "util/io.h"
#include "util/htable.h"

typedef struct {
    uint32_t p1res, p2res;
    size_t nnums;
    uint32_t nums[];
} input;

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day15/", filename, "r");

	char *s = fslurp(f, "\r");

	fclose(f);

	size_t nnums = 1;

	for (size_t i = 0; s[i]; i += 1) {
		if (s[i] == ',') {
			nnums += 1;
		}
	}

	input *ret = malloc(sizeof (input) + sizeof(uint32_t[nnums]));
	ret->nnums = nnums;

	char *w = s;

	for (size_t i = 0; i < nnums; i += 1) {
		ret->nums[i] = strtoul(w, &w, 10);
		w += 1;
	}

	free(s);
	return (void *)ret;
}
uint32_t seen[30000000];

static void do_rounds(input *inp, uint32_t nrounds)
{


	uint32_t i, lastnum, lastseen;
	//uint32_t *seen = calloc(nrounds + 1, sizeof (uint32_t));

	for (i = 1; i <= inp->nnums; i += 1) {
		lastnum = inp->nums[i - 1];
		seen[lastnum] = i;
	}

	for (; i < 2020; i += 1) {
		if ((lastseen = seen[lastnum])) {
			seen[lastnum] = i;
			lastnum = i - lastseen;
		} else {
			seen[lastnum] = i;
			lastnum = 0;
		}
	}
	inp->p1res = lastnum;

	for (; i < nrounds; i += 1) {
		if ((lastseen = seen[lastnum])) {
			seen[lastnum] = i;
			lastnum = i - lastseen;
		} else {
			seen[lastnum] = i;
			lastnum = 0;
		}
	}

	inp->p2res = lastnum;
	//free(seen);

}

static result *part1(void *data)
{
	input *inp = data;



	do_rounds(inp, 30000000);
	return_result((uint64_t) inp->p1res);
}

static result *part2(void *data)
{
	input *inp = data;

	return_result((uint64_t)inp->p2res);
}

day day15 = {
	getinput,
	part1,
	part2,
	free
};