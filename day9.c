#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/htable.h"
#include "util/io.h"
#include "day.h"

typedef struct {
    size_t n;
    size_t npreamb;
    uint64_t target;
    uint64_t nums[];
} input;

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day9/", filename, "r");

	char **lines;
	size_t nlines = get_lines(f, &lines);

	fclose(f);

	input *r = malloc(sizeof (input) + sizeof (uint64_t[nlines]));
	r->npreamb = strcmp(filename, "example2") == 0 ? 5 : 25;

	for (size_t i = 0; i < nlines; i += 1) {
		r->nums[i] = strtoull(lines[i], NULL, 10);
		free(lines[i]);
	}

	free(lines);
	r->n = nlines;
	return (void *)r;
}



static result *part1(void *data)
{
	input *inp = data;
	size_t n = inp->n;
	size_t npreamb = inp->npreamb;
	uint64_t *nums = inp->nums;

	uint64_t *preamb = calloc(npreamb, sizeof(uint64_t));
	htable_t *ht = ht_create(&u64hash, &u64cmp, NULL);


	size_t i, j, k;
	uint64_t key, v;

	uint64_t res = 0;

	for (i = 0; i < npreamb; i += 1) {
		preamb[i] = nums[i];
		ht_insert(ht, preamb + i, NULL);
	}

	for (; i < n; i += 1) {
		k = i % npreamb;
		v = nums[i];

		for (j = 0; j < npreamb; j += 1) {
			key = preamb[j];
			key = (key < v) ? v - key : key - v;
			if (ht_get(ht, &key, NULL)) {
				break;
			}
		}

		if (j < npreamb) {
			ht_remove(ht, preamb + k);
			preamb[k] = v;
			ht_insert(ht, preamb + k, NULL);
		} else {
			res = v;
			goto ret;
		}
	}

	ret:
	inp->target = res;
	ht_destroy(ht);
	free(preamb);
	return_result(res);
}

static result *part2(void *data)
{
	input *inp = data;
	size_t n = inp->n;
	uint64_t diff = inp->target;
	uint64_t *nums = inp->nums;

	size_t head, tail;

	for (head = tail = 0; tail < n;) {
		if (diff == nums[tail]) {
			break;
		} else if (diff < nums[tail]) {
			diff += nums[head++];
		} else {
			diff -= nums[tail++];
		}
	}

	uint64_t min, max;
	size_t i;
	min = max = nums[head];

	for (i = head; i < tail; i += 1) {
		if (nums[i] < min) {
			min = nums[i];
		} else if (nums[i] > max) {
			max = nums[i];
		}
	}

	return_result(min + max);
}

day day9 = {
	getinput,
	part1,
	part2,
	free
};