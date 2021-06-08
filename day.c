#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#include "day.h"
#include "flags.h"

typedef enum result_type {
    UINT,
    SINT,
    STRING,
    FDOUBLE
} result_type;

struct result {
    result_type type;
    union {
	uint64_t uint;
	int64_t sint;
	char *string;
	double fdouble;
    } data;
};

result *make_result_uint(uint64_t r)
{
	result *ret = calloc(1, sizeof (result));
	ret->type = UINT;
	ret->data.uint = r;

	return ret;
}

result *make_result_sint(int64_t r)
{
	result *ret = calloc(1, sizeof (result));
	ret->type = SINT;
	ret->data.sint = r;

	return ret;
}

result *make_result_double(double r)
{
	result *ret = calloc(1, sizeof (result));
	ret->type = FDOUBLE;
	ret->data.fdouble = r;

	return ret;
}

result *make_result_string(char *r)
{
	result *ret = calloc(1, sizeof (result));
	ret->type = STRING;
	ret->data.string = r;

	return ret;
}

static inline void print_result(result *r)
{
	switch (r->type) {
		case UINT:
			printf("%lu", r->data.uint);
			break;
		case SINT:
			printf("%ld", r->data.sint);
			break;
		case STRING:
			printf("%s", r->data.string);
			break;
		case FDOUBLE:
			printf("%f", r->data.fdouble);
			break;
	}
}

static inline void freeresult(void *in)
{
	result *r = in;

	if (r->type == STRING) {
		free(r->data.string);
	}
	free(r);
}
extern day day1;
extern day day2;
extern day day3;
extern day day4;
extern day day5;
extern day day6;
extern day day7;
extern day day8;
extern day day9;
extern day day10;
extern day day11;
extern day day12;
extern day day13;
extern day day14;
extern day day15;
extern day day16;
extern day day17;
extern day day18;
extern day day19;
extern day day20;
extern day day21;
extern day day22;
extern day day23;
extern day day24;
extern day day25;

day *days[25];

enum {
    d1,
    d2,
    d3,
    d4,
    d5,
    d6,
    d7,
    d8,
    d9,
    d10,
    d11,
    d12,
    d13,
    d14,
    d15,
    d16,
    d17,
    d18,
    d19,
    d20,
    d21,
    d22,
    d23,
    d24,
    d25,
};

static void run_day_(day *day, char *filename);

void init_days()
{
	days[d1] = &day1;
	days[d2] = &day2;
	days[d3] = &day3;
	days[d4] = &day4;
	days[d5] = &day5;
	days[d6] = &day6;
	days[d7] = &day7;
	days[d8] = &day8;
	days[d9] = &day9;
	days[d10] = &day10;
	days[d11] = &day11;
	days[d12] = &day12;
	days[d13] = &day13;
	days[d14] = &day14;
	days[d15] = &day15;
	days[d16] = &day16;
	days[d17] = &day17;
	days[d18] = &day18;
	days[d19] = &day19;
	days[d20] = &day20;
	days[d21] = &day21;
	days[d22] = &day22;
	days[d23] = &day23;
	days[d24] = &day24;
	days[d25] = &day25;
}

void run_all()
{
	clock_t t = 0;
	if (flags & PROFILE) {
		t = clock();
	}

	for (int i = d1; i <= d25; i += 1) {
		run_day(i);
	}
	if (flags & PROFILE) {
		t = clock() - t;
		float secs = (float)t / CLOCKS_PER_SEC;
		printf("\tRun_all completed in %ld ticks(%f seconds).\n", t, secs);
	}
}

void run_last()
{
	for (int i = d25; i >= d1; i -= 1) {
		if (days[i] != NULL) {
			run_day(i);
			break;
		}
	}


}

static int filterinput(const struct dirent *d)
{
	if (d->d_name[0] == '.') return 0;
	return strncmp(d->d_name, "input", 5);
}

static void run_examples(int idx)
{
	char *path;
	asprintf(&path, "../inputs/day%d/", idx + 1);
	struct dirent **d;
	size_t nexamples = scandir(path, &d, filterinput, alphasort);

	for (size_t i = 0; i < nexamples; i += 1) {
		run_day_(days[idx], d[i]->d_name);
	}

	free(path);
	for (size_t i = -1; i < nexamples; i += 1) {
		free(d[i]);
	}
	free(d);
}

static void run_day_(day *d, char *filename)
{
	clock_t t = 0;
	if (flags & PROFILE) {
		t = clock();
	}
	void *data = d->getinput(filename);
	printf("\tPart 1 %s: ", filename);
	result *p1res = d->part1(data);
	print_result(p1res);

	printf("\n\tPart 2 %s: ", filename);
	result *p2res = d->part2(data);
	print_result(p2res);
	putchar('\n');

	if (flags & PROFILE) {
		t = clock() - t;
		float secs = (float)t / CLOCKS_PER_SEC;
		printf("\tCompleted in %ld ticks(%f seconds).\n", t, secs);
	}
	free(p1res);
	free(p2res);
	d->freedata(data);
}


void run_day(int idx)
{
	if (idx < d1 || idx > d25) {
		fprintf(stderr, "Invalid day: %d\n", idx + 1);
		return;
	} else if (days[idx] == NULL) {
		fprintf(stderr, "No solution for day %d yet.\n", idx + 1);
		return;
	}

	printf("Day %d:\n", idx + 1);

	if (flags & RUN_EXAMPLES) {
		run_examples(idx);
	} else {
		run_day_(days[idx], "input");
	}
}




