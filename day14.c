#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "day.h"
#include "util/io.h"
#include "util/htable.h"



typedef struct {
    uint64_t on;
    uint64_t off;
    uint64_t floats;
} mask;
void parse_mask(const char *s, mask *m)
{
	m->on = 0;
	m->off = 0;
	m->floats = 0;
	uint64_t bit = 1ul << 35;

	while (*s) {
		switch(*s) {
			case '1':
				m->on |= bit;
				break;
			case '0':
				m->off |= bit;
				break;
			case 'X':
				m->floats |= bit;
		}
		s += 1;
		bit >>= 1;
	}
}
void apply_mask(mask *m, uint64_t *v) {
	*v |= m->on;
	*v &= ~m->off;
}

typedef enum {
    MASK,
    WRITE
} optype;

typedef struct {
	optype type;
	union {
	    mask m;
	    struct {
	        uint64_t address;
	        uint64_t val;
	    };
	};
} op;

typedef struct {
    size_t nops;
    op ops[];
} input;

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day14/", filename, "r");
	char **lines;
	size_t nlines = get_lines(f, &lines);
	fclose(f);

	input *res = malloc(sizeof(input) + sizeof(op[nlines]));
	op *p;


	for (size_t i = 0; i < nlines; i += 1) {
		p = res->ops + i;
		if (strncmp(lines[i], "mask", 4) == 0) {
			p->type = MASK;
			parse_mask(lines[i] + 7, &p->m);
		} else {
			p->type = WRITE;
			sscanf(lines[i], "mem[%lu] = %lu", &p->address, &p->val);
		}

		free(lines[i]);
	}

	free(lines);
	res->nops = nlines;

	return (void *)res;
}

void
write_all(htable_t *mem, uint64_t val, mask *m, uint64_t addr, uint64_t bit)
{

	for (;;) {
		if (!bit) {
			ht_insert(mem, &addr, &val);
			return;
		} else if (m->on & bit) {
			addr |= bit;
		} else if (m->floats & bit) {
			write_all(mem, val, m, addr, bit >> 1);
			write_all(mem, val, m, addr ^ bit, bit >> 1);
			return;
		}

		bit >>= 1;
	}
}
void run_op(op *o, mask **m, htable_t *mem, bool usev2)
{
	if (o->type == MASK) {
		*m = &o->m;
	} else if (usev2) {
		write_all(mem, o->val, *m, o->address, 1ul << 35);
	} else {
		uint64_t v = o->val;
		apply_mask(*m, &v);
		ht_insert(mem, &o->address, &v);
	}

}

uint64_t run_program(input *prog, bool usev2)
{
	ht_callbacks cbs = {
		u64cpy,
		free,
		u64cpy,
		free
	};
	htable_t *mem = ht_create(u64hash, u64cmp, &cbs);
	mask *m;

	for (size_t i = 0; i < prog->nops; i += 1) {
		run_op(prog->ops + i, &m, mem, usev2);
	}

	ht_enum_t *he = ht_enum_create(mem);

	uint64_t *val;
	uint64_t res = 0;

	while(ht_enum_next(he, NULL, (void **)&val)) {
		res += *val;
	}

	ht_enum_destroy(he);
	ht_destroy(mem);
	return res;
}

static result *part1(void *data)
{
	return_result(run_program(data, false));
}

static result *part2(void *data)
{
	return_result(run_program(data, true));
}

day day14 = {
	getinput,
	part1,
	part2,
	free,
};