#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>

#include "day.h"
#include "util/io.h"

typedef struct {
    size_t nlines;
    char **lines;
} input;

static void freeinput(void *in)
{
	input *input = in;

	for (size_t i = 0; i < input->nlines; i += 1) {
		free(input->lines[i]);
	}

	free(input->lines);
	free(input);
}

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day18/", filename, "r");

	input *res = malloc(sizeof (input));

	res->nlines = get_lines(f, &res->lines);

	fclose(f);

	return (void *) res;
}
typedef enum {
    ADD = '+',
    MUL = '*',
} state;

static uint64_t comp_exp(char **exp, bool do_prec)
{
	state op = do_prec ? MUL : ADD;
	uint64_t acc = do_prec ? 1 : 0;
	uint64_t accarg = 0;
	uint64_t arg;

	char *w = *exp;
	while (*w) {
		if (isspace(*w)) {
			w += 1;
			continue;
		} else if (*w == ')') {
			*exp = w + 1;
			break;
		} else if (*w == '(') {
			w += 1;
			arg = comp_exp(&w, do_prec);
		} else if (ispunct(*w)) {
			op = *w++;
			continue;
		} else if (isdigit(*w)) {
			arg = strtoul(w, &w, 10);
		}

		if (do_prec) {
			if (op == MUL && accarg > 0) {
				acc *= accarg;
				accarg = arg;
			} else {
				accarg += arg;
			}
		} else {
			acc = (op == ADD) ? acc + arg : acc * arg;
		}
	}

	if (accarg > 0) {
		acc *= accarg;
	}

	return acc;
}

static result *part1(void *data)
{
	input *inp = data;
	uint64_t res = 0;

	for (size_t i = 0; i < inp->nlines; i += 1) {
		res += comp_exp(&inp->lines[i], false);
	}

	return_result(res);
}

static result *part2(void *data)
{
	input *inp = data;
	uint64_t res = 0;

	for (size_t i = 0; i < inp->nlines; i += 1) {
		res += comp_exp(&inp->lines[i], true);
	}

	return_result(res);
}

day day18 = {
	getinput,
	part1,
	part2,
	freeinput,
};