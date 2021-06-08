#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/io.h"
#include "day.h"

typedef enum {
    NUL = 0,
    NOP,
    ACC,
    JMP
} opcode;

static char *opnames[] = {"nul", "nop", "acc", "jmp"};

typedef struct {
    opcode op;
    int64_t arg;
    int64_t acc;
    bool seen;
} instruction;

static void printi(size_t ip, instruction *p)
{
	printf("%lu: %s %d\n", ip, opnames[p->op], p->arg);
}

void parse_instruction(char *s, instruction *inst)
{
	if (strncmp(s, "nop", 3) == 0) {
		inst->op = NOP;
	} else if (strncmp(s, "acc", 3) == 0) {
		inst->op = ACC;
	} else if (strncmp(s, "jmp", 3) == 0) {
		inst->op = JMP;
	}

	inst->arg = strtol(s + 4, NULL, 10);
}

typedef struct {
    size_t ninstructions;
    instruction program[];
} input;

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day8/", filename, "r");
	char **lines;
	size_t nlines = get_lines(f, &lines);
	fclose(f);

	input *res = calloc(1, sizeof (input) + sizeof (instruction[nlines + 1]));

	for (size_t i = 0; i < nlines; i += 1) {
		parse_instruction(lines[i], res->program + i);
		free(lines[i]);
	}
	free(lines);

	res->ninstructions = nlines + 1;
	return (void *)res;
}

static bool exec_op(opcode op, int64_t arg, int64_t *acc, size_t *ip)
{
	switch (op) {
		case ACC:
			*acc += arg;
		case NOP:
			*ip += 1;
			break;
		case JMP:
			*ip += arg;
			break;
		case NUL:
			return true;
	}

	return false;
}

static result *part1(void *data)
{
	input *inp = data;
	instruction *program = inp->program;
	int64_t acc = 0;
	size_t ip = 0;
	instruction *p;

	for (;;) {
		p = program + ip;

		if (p->seen) {
			break;
		} else {
			p->acc = acc;
			p->seen = true;
		}

		exec_op(p->op, p->arg, &acc, &ip);
	}

	return_result(acc);
}

static result *part2(void *data)
{
	int64_t acc = 0;
	size_t ip = 0;

	input *inp = data;
	size_t n = inp->ninstructions;
	instruction *program = inp->program;

	size_t *to_try = calloc(n, sizeof(size_t));

	size_t ntry;
	size_t i;
	instruction *p;

	/* Enumerate all the instructions we need to try to flip.
	 * Since we can only flip one instruction,
     	 * it only makes sense to try the ones executed in part 1.
     	 * They've been tagged for us already by part 1 */
	for (i = 0, ntry = 0; i < n; i += 1) {
		p = &program[i];

		if (p->seen && (p->op == NOP || p->op == JMP)) {
			to_try[ntry] = i;
			ntry += 1;
		}
	}

	for (i = 0; i < ntry; i += 1) {
		ip = to_try[i];
		p = program + ip;
		acc = p->acc;
		if (exec_op(p->op == JMP ? NOP : JMP, p->arg, &acc, &ip)) {
			goto ret;
		}

		for (;;) {
			if (ip > n || (p = program + ip)->seen) {
				break;
			} else {
				p->seen = true;
				p->acc = acc;
			}

			if (exec_op(p->op, p->arg, &acc, &ip)) {
				goto ret;
			}
		}
	}

	ret:
	free(to_try);
	return_result(acc);
}

day day8 = {
	getinput,
	part1,
	part2,
	free
};