#include "util/io.h"
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>

#include "day.h"

typedef complex double direction;
typedef complex double position;

static const direction north = I;
static const direction south = -I;
static const direction east = 1;
static const direction west = -1;

typedef struct {
    direction direction;
    position pos;
    position wpoint;
} ship;

static ship *newship()
{
	ship *s = malloc(sizeof (ship));
	s->direction = east;
	s->pos = 0;
	s->wpoint = 0;

	return s;
}

static inline uint64_t manhattan(ship *s)
{
	double rpos = round(creal(s->pos));
	uint64_t rposabs = (uint64_t)fabs(rpos);
	double ipos = round(cimag(s->pos));
	uint64_t iposabs = (uint64_t)fabs(ipos);

	return rposabs + iposabs;
}

enum {
    movenorth = 'N',
    movesouth = 'S',
    moveeast = 'E',
    movewest = 'W',
    right = 'R',
    left = 'L',
    forward = 'F'
};
typedef char op;

typedef struct {
    op op;
    int arg;
} instruction;

typedef struct {
    size_t ninstructions;
    instruction instructions[];
} input;

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day12/", filename, "r");
	char **lines;
	size_t nlines = get_lines(f, &lines);
	input *res = malloc(sizeof (input) + sizeof (instruction[nlines]));

	for (size_t i = 0; i < nlines; i += 1) {
		res->instructions[i].op = lines[i][0];
		res->instructions[i].arg = (int)strtol(lines[i] + 1, NULL, 10);
		free(lines[i]);
	}

	free(lines);
	res->ninstructions = nlines;
	return (void *)res;
}



static void move(position *pos, direction d, position distance)
{
	*pos += d * distance;
}

static void rotate(ship *s, double angle)
{
	position rot_coeff = cpow(copysign(1.0f, angle) * I, fabs(angle) / 90);
	s->direction *= rot_coeff;
	s->wpoint *= rot_coeff;
}

static void do_instruction(ship *s, instruction *inst, bool usewaypoint)
{
	position *pos = usewaypoint ? &s->wpoint : &s->pos;
	direction dir = usewaypoint ? s->wpoint : s->direction;

	switch (inst->op) {
		case movenorth:
			move(pos, north, inst->arg);
			break;
		case movesouth:
			move(pos, south, inst->arg);
			break;
		case moveeast:
			move(pos, east, inst->arg);
			break;
		case movewest:
			move(pos, west, inst->arg);
			break;
		case forward:
			move(&s->pos, dir, inst->arg);
			break;
		case right:
			rotate(s, -(inst->arg));
			break;
		case left:
			rotate(s, inst->arg);
			break;
	}
}

static result *part1(void *data)
{
	input *inp = data;
	size_t ninst = inp->ninstructions;
	instruction *inst = inp->instructions;
	ship *s = newship();

	for (size_t i = 0; i < ninst; i += 1) {
		do_instruction(s, inst + i, false);
	}

	return_result(manhattan(s));
}

static result *part2(void *data)
{
	input *inp = data;
	size_t ninst = inp->ninstructions;
	instruction *inst = inp->instructions;

	ship *s = newship();
	s->wpoint = 10 + I;

	for (size_t i = 0; i < ninst; i += 1) {
		do_instruction(s, inst + i, true);
	}

	return_result(manhattan(s));
}

day day12 = {
	getinput,
	part1,
	part2,
	free
};