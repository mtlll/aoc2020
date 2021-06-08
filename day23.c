#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "day.h"
#include "util/io.h"
#include "util/list.h"

typedef uint32_t num_t;


typedef struct {
    num_t p1[10];
    num_t p2[1000001];
} input;

static void freeinput(void *in)
{
	free(in);
}

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day23/", filename, "r");
	char *s = fslurp(f, "\r");
	fclose(f);
	input *ret = malloc(sizeof (input));

	num_t tmp[9];
	size_t i;
	for (i = 0; s[i]; i += 1) {
		tmp[i] = s[i] - '0';
	}

	free(s);

	for (i = 1; i < 1000000; i += 1) {
		ret->p2[i] = i + 1;
	}
	ret->p1[0] = ret->p2[0] = tmp[0];
	for (i = 0; i < 8; i += 1) {
		ret->p1[tmp[i]] = ret->p2[tmp[i]] = tmp[i + 1];
	}
	ret->p1[tmp[8]] = tmp[0];
	ret->p2[tmp[8]] = 10;
	ret->p2[1000000] = tmp[0];


	return (void *)ret;
}


static void do_turns(num_t *cups, size_t nturns, num_t max)
{
	num_t cur, target, c1, c2, c3;


	cur = 0;
	while (nturns--) {
		//list_cut_position_dirty(&subl, cur, cur->next->next->next);
		cur = cups[cur];
		target = cur;
		c1 = cups[cur];
		c2 = cups[c1];
		c3 = cups[c2];

		do {
			target = (target > 1) ? target - 1 : max;
		} while (target == c1 || target == c2 || target == c3);

		cups[cur] = cups[c3];
		cups[c3] = cups[target];
		cups[target] = c1;
	}

}

static result *part1(void *data)
{

	num_t *cups = ((input *) data)->p1;

	do_turns(cups, 100, 9);

	char *ret;
	char *tmp = strdup("");
	for (num_t cur = cups[1]; cur != 1; cur = cups[cur]) {
		asprintf(&ret, "%s%u", tmp, cur);
		free(tmp);
		tmp = ret;
	}

	return_result(ret);
}

static result *part2(void *data)
{
	num_t *cups = ((input *) data)->p2;

	do_turns(cups, 10000000, 1000000);

	uint64_t res = (uint64_t) cups[1];
	res *= (uint64_t) cups[res];

	return_result(res);
}

day day23 = {
	getinput,
	part1,
	part2,
	freeinput,
};