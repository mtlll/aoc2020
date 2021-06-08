#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/io.h"
#include "day.h"

enum {
    floor = '.',
    empty = 'L',
    emptying = 'E',
    occupied = '#',
    reserved = 'R'
};

typedef char seat;

typedef struct {
    size_t nrows;
    size_t ncols;
    bool skipfloor;
    unsigned int tolerance;
    seat **seats;
} seatgrid;

static void *getinput(char *filename)
{
	seatgrid *g = calloc(1, sizeof(seatgrid));
	FILE *f = fopen_in_path("../inputs/day11/", filename, "r");

	g->nrows = get_lines(f, &g->seats);
	g->ncols = strlen(g->seats[0]);

	fclose(f);
	return (void *)g;
}

static void sgreset(seatgrid *g)
{
	seat *p;
	for (size_t i = 0; i < g->nrows; i += 1) {
		for (size_t j = 0; j < g->ncols; j += 1) {
			p = &g->seats[i][j];
			switch (*p) {
				case floor:
					continue;
				default:
					*p = empty;
			}
		}
	}
}

static void sgfree(void *in)
{
	if (in == NULL) return;

	seatgrid *g = in;

	for (size_t i = 0; i < g->nrows; i += 1) {
		free(g->seats[i]);
	}

	free(g->seats);
	free(g);
}

static bool
is_occupied(seatgrid *g, size_t row, ptrdiff_t drow, size_t col, ptrdiff_t dcol)
{
	size_t newrow = row;
	size_t newcol = col;
	bool isbehind = (drow < 0 || (drow == 0 && dcol < 0)) ? true : false;
	seat *s;

	do {
		newrow += drow;
		newcol += dcol;
		if (newrow >= g->nrows) return false;
		if (newcol >= g->ncols) return false;
		s = &g->seats[newrow][newcol];

		if (*s == occupied) return true;
		if (isbehind && *s == emptying) return true;
		if (!isbehind && *s == reserved) return true;
	} while (g->skipfloor && *s == floor);


	return false;
}

static unsigned int count_adjacent(seatgrid *g, size_t row, size_t col)
{
	return is_occupied(g, row, -1, col, -1) +
	       is_occupied(g, row, -1, col, 0) +
	       is_occupied(g, row, -1, col, 1) +
	       is_occupied(g, row, 0, col, -1) +
	       is_occupied(g, row, 0, col, 1) +
	       is_occupied(g, row, 1, col, -1) +
	       is_occupied(g, row, 1, col, 0) +
	       is_occupied(g, row, 1, col, 1);
}

static uint64_t count_occupied(const seatgrid *g)
{
	size_t i, j;
	int count = 0;

	for (i = 0; i < g->nrows; i += 1) {
		for (j = 0; j < g->ncols; j += 1) {
			if (g->seats[i][j] == occupied) {
				count += 1;
			}
		}
	}
	return count;
}

static unsigned int evolve_grid(seatgrid *g)
{
	size_t i, j;
	seat *p;
	unsigned int changes = 0;

	for (i = 0; i < g->nrows; i += 1) {
		for (j = 0; j < g->ncols; j += 1) {
			p = &(g->seats[i][j]);
			switch (*p) {
				case floor:
					continue;
				case emptying:
				case empty:
					if (count_adjacent(g, i, j) == 0) {
						*p = reserved;
						changes += 1;
					} else {
						*p = empty;
					}
					break;
				case reserved:
				case occupied:
					if (count_adjacent(g, i, j) >=
					    g->tolerance) {
						*p = emptying;
						changes += 1;
					} else {
						*p = occupied;
					}
					break;
			}
		}
	}
	return changes;
}

static void find_equilibrium(seatgrid *g)
{
	while (evolve_grid(g));
}

static result *part1(void *data)
{
	seatgrid *g = data;
	g->tolerance = 4;
	g->skipfloor = false;

	find_equilibrium(g);

	return_result(count_occupied(g));
}

static result *part2(void *data)
{
	seatgrid *g = data;
	sgreset(g);
	g->tolerance = 5;
	g->skipfloor = true;

	find_equilibrium(g);

	return_result(count_occupied(g));
}

day day11 = {
	getinput,
	part1,
	part2,
	sgfree
};

