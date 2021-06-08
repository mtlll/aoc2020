#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/io.h"
#include "day.h"

#define TREE '#'

typedef struct {
    size_t nlines;
    size_t nrows;
    char **m;
} map;
static void freeinput(void *in)
{
	map *map = in;
	for (size_t i = 0; i < map->nlines; i += 1) {
		free(map->m[i]);
	}
	free(map);
}
static void *getinput(char *filename)
{
	map *r = malloc(sizeof (map));
	FILE *f = fopen_in_path("../inputs/day3/", filename, "r");
	r->nlines = get_lines(f, &r->m);
	r->nrows = strlen(r->m[0]);
	fclose(f);

	return (void *)r;
}

static unsigned long count_trees(map *map, size_t linestep, size_t rowstep)
{
	char **m = map->m;
	size_t nl = map->nlines;
	size_t nr = map->nrows;
	size_t i, j;
	unsigned long res = 0;
	for (i = j = 0; i < nl; i += linestep, j = (j + rowstep) % nr) {
		if (m[i][j] == TREE) {
			res += 1;
		}
	}
	return res;
}

static result *part1(void *data)
{
	return_result(count_trees((map *)data, 1, 3));
}

static result *part2(void *data)
{
	unsigned long res = 1;
	map *map = data;

	res *= count_trees(map, 1, 1);
	res *= count_trees(map, 1, 3);
	res *= count_trees(map, 1, 5);
	res *= count_trees(map, 1, 7);
	res *= count_trees(map, 2, 1);

	return_result(res);
}

day day3 = {
	getinput,
	part1,
	part2,
	freeinput
};