#include <assert.h>
#include <stdlib.h>

#include "util/io.h"
#include "day.h"

typedef struct seat {
    char *s;
    int row;
    int col;
    unsigned long seatID;
} seat;

static int seatIDcmp(const void *a, const void *b)
{
	const seat *aa =  a;
	const seat *bb =  b;

	return aa->seatID - bb->seatID;
}

static void compute_seat(seat *seat)
{
	int rowmin = 0;
	int rowmax = 127;
	int rowjump = 64;
	int colmin = 0;
	int colmax = 7;
	int coljump = 4;

	char *s = seat->s;

	while (*s) {
		switch (*s) {
			case 'F':
				rowmax -= rowjump;
				rowjump >>= 1;
				break;
			case 'B':
				rowmin += rowjump;
				rowjump >>= 1;
				break;
			case 'L':
				colmax -= coljump;
				coljump >>= 1;
				break;
			case 'R':
				colmin += coljump;
				coljump >>= 1;
				break;
		}
		s += 1;
	}

	assert(rowmax == rowmin);
	assert(colmax == colmin);

	seat->row = rowmax;
	seat->col = colmax;
}

static int seat_ID(seat *seat)
{
	return seat->row * 8 + seat->col;
}

typedef struct {
    size_t nseats;
    seat seats[];
} input;

static void freeinput(void *in)
{
	input *inp = in;
	for (size_t i = 0; i < inp->nseats; i += 1) {
		free(inp->seats[i].s);
	}
	free(inp);
}

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day5/", filename, "r");

	char **lines;
	size_t nlines = get_lines(f, &lines);

	fclose(f);

	input *r = malloc(sizeof(input) + sizeof (seat[nlines]));
	seat *s;

	for (size_t i = 0; i < nlines && *lines[i]; i += 1) {
		s = r->seats + i;
		s->s = lines[i];
		compute_seat(s);
		s->seatID = seat_ID(s);
	}

	free(lines);

	qsort(r->seats, nlines, sizeof(seat), seatIDcmp);

	r->nseats = nlines;
	return (void *)r;
}

static void printseat(struct seat *s)
{
	printf("%s: row %d, col %d, seatID %lu\n", s->s, s->row,
	       s->col, s->seatID);
}

static result *part1(void *data)
{
	input *inp = data;

	return_result(inp->seats[inp->nseats - 1].seatID);
}

static result *part2(void *data)
{
	input *inp = data;
	size_t nseats = inp->nseats;
	struct seat *seats = inp->seats;
	struct seat *cur;
	unsigned long lastID = seats[0].seatID;

	for (size_t i = 1; i < nseats; i += 1) {
		cur = &seats[i];
		if (cur->seatID - lastID == 2) {
			return_result(lastID + 1);
		}
		lastID = cur->seatID;
	}

	return_result(0ul);
}

day day5 = {
	getinput,
	part1,
	part2,
	freeinput,
};