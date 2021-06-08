#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "day.h"
#include "flags.h"
#include "util/io.h"
#include "util/ndarray.h"

static char state[4] = {'#', '.', 'd', 'w',};

enum {
    ACTIVE = 0,
    INACTIVE = 1,
    DYING = 2,
    WAKING = 3,
};

static inline size_t max_size(size_t startsize, size_t ncycles)
{
	return startsize + (ncycles * 2) + 2;
}


typedef struct {
    char **init;
    size_t len;

} input;
static void freeinput(input *inp)
{
	for (size_t i = 0; i < inp->len; i += 1) {
		free(inp->init[i]);
	}
	free(inp->init);
	free(inp);
}

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day17/", filename, "r");
	char *s = fslurp(f, "\r");
	fclose(f);

	input *r = calloc(1, sizeof (input));

	char *w = s;
	char *tok;
	size_t i = 0;

	while ((tok = strsep(&w, "\n")) && *tok) {
		if (!r->init) {
			r->len = strlen(tok);
			r->init = reallocarray(NULL, r->len, sizeof (char *));
		}

		r->init[i++] = strdup(tok);
	}

	free(s);

	return (void *) r;
}

static void print_grid(size_t ni, size_t nj, ndarray_t *grid);


static ndarray_t *
create_grid(const input *inp, size_t ncycles, size_t ndimensions)
{
	ndarray_t *grid;
	if (ndimensions == 2) {
		grid = nda_create(ndimensions,
		    		  sizeof (char),
		    		  max_size(inp->len, ncycles),
		    		  max_size(inp->len, ncycles));
	} else if (ndimensions == 3) {
		grid = nda_create(ndimensions,
		                  sizeof (char),
		                  max_size(1, ncycles),
		                  max_size(inp->len, ncycles),
		                  max_size(inp->len, ncycles));
	} else if (ndimensions == 4) {
		grid = nda_create(ndimensions,
				  sizeof (char),
				  max_size(1, ncycles),
				  max_size(1, ncycles),
				  max_size(inp->len, ncycles),
				  max_size(inp->len, ncycles));
	}

	nda_set_all(grid, &state[INACTIVE]);
	return grid;
}

static ndarray_t *
setup_grid(const input *inp, size_t ncycles, size_t ndimensions)
{
	ndarray_t *grid = create_grid(inp, ncycles, ndimensions);


	size_t center[ndimensions];
	for (size_t i = 0; i < ndimensions; i += 1) {
		center[i] = ncycles + 1;
	}

	for (size_t i = 0; i < inp->len; i += 1) {
		center[ndimensions - 2] = ncycles + 1 + i;
		strncpy(nda_get(grid, center), inp->init[i], inp->len);
	}

	return grid;
}
static inline bool is_active(nda_enum_t *nde)
{
	char *cur = nda_enum_get(nde, NULL);
	return *cur == state[ACTIVE];
}
static uint32_t count_neighbours(nda_enum_t *nde) {
	uint32_t count = 0;
	nde_neighbour_enum(nde);

	do {
		count += is_active(nde);
	} while (nda_enum_next(nde));

	return count;
}
static void evolve_grid(nda_enum_t *nde, ndarray_t *new)
{
	//nda_enum_t
	char *curnew, *curold;
	uint32_t nneighbours;

	do {
		curold = nda_enum_get(nde, NULL);
		curnew = nda_enum_get(nde, new);
		nneighbours = count_neighbours(nde);
		switch(*curold) {
			case '#':
				if (nneighbours < 2 || nneighbours > 3) {
					*curnew = state[INACTIVE];
				} else {
					*curnew = state[ACTIVE];
				}
				break;
			case '.':
				if (nneighbours == 3) {
					*curnew = state[ACTIVE];
				} else {
					*curnew = state[INACTIVE];
				}
				break;
		}
	} while (nda_enum_next(nde));
}

static ndarray_t *do_cycles(size_t ncycles, ndarray_t *old, ndarray_t *new)
{
	nda_enum_t *nde;
	bool tick = true;
	if (flags & VERBOSE) {
		printf("\n");
		print_grid(nda_extent(old, 0),
			   nda_extent(old, 1),
			   old);
	}
	for (size_t cyc = 0; cyc < ncycles; cyc += 1)
	{


		if (tick) {
			nde = nda_enum_create(old, ncycles - cyc, NULL);
			evolve_grid(nde, new);
			print_grid(nda_extent(new, 0),
				   nda_extent(new, 1),
				   new);
		} else {

			nde = nda_enum_create(new, ncycles - cyc, NULL);
			evolve_grid(nde, old);
			print_grid(nda_extent(old, 0),
				   nda_extent(old, 1),
				   old);
		}

		tick = !tick;
		nda_enum_destroy(nde);
	}

	if (tick) {
		nda_destroy(new);
		return old;
	} else {
		nda_destroy(old);
		return new;
	}
}

static uint64_t count_active(ndarray_t *grid)
{
	uint64_t cnt = 0;
	char *cur;
	nda_enum_t *nde = nda_enum_create(grid, 1, NULL);

	do {
		cur = nda_enum_get(nde, NULL);
		if (*cur == state[ACTIVE]) {
			cnt += 1;
		}
	} while (nda_enum_next(nde));

	nda_enum_destroy(nde);
	return cnt;
}

static uint64_t
boot_process(const input *inp, size_t ncycles, size_t ndimensions)
{
	uint64_t cnt;
	ndarray_t *old = setup_grid(inp, ncycles, ndimensions);
	ndarray_t *new = create_grid(inp, ncycles, ndimensions);
	ndarray_t *final;
	final = do_cycles(ncycles, old, new);

	cnt = count_active(final);

	nda_destroy(final);
	return cnt;
}

static result *part1(input *inp)
{
	size_t ncycles = 6;
	size_t ndimensions = (flags & VERBOSE) ? 2 : 3;
	uint64_t cnt = boot_process(inp, ncycles, ndimensions);

	return_result(cnt);
}

static void print_grid(size_t ni, size_t nj, ndarray_t *grid)
{
	if (flags & VERBOSE) {
		size_t idx[2];
		size_t *i = &idx[0];
		size_t *j = &idx[1];
		char *c;
		for (*i = 0; *i < ni; *i += 1) {
			printf("\n");
			for (*j = 0; *j < nj; *j += 1) {
				c = nda_get(grid, idx);
				printf("%c", *c);
			}
		}
	}
}

static result *part2(input *inp)
{
	return_result(boot_process(inp, 6, 4));
}

day day17 = {
	getinput,
	part1,
	part2,
	freeinput,
};