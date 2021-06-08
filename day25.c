#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "day.h"
#include "util/io.h"

typedef struct {
    uint64_t card_key;
    uint64_t door_key;
} input;

static void freeinput(void *in)
{
	free(in);
}

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day25/", filename, "r");
	char *s = fslurp(f, "\r");
	fclose(f);
	char *next;

	input *ret = malloc(sizeof (input));

	ret->card_key = strtoul(s, &next, 10);
	ret->door_key = strtoul(next + 1, NULL, 10);

	free(s);

	return (void *)ret;
}

uint64_t powermod(uint64_t base, uint64_t exponent, uint64_t mod)
{
	uint64_t res = 1;

	if (base >= mod) {
		exponent %= mod;
	}

	if (base == 0) {
		return 0;
	}

	while (exponent > 0)
	{
		if (exponent & 1) {
			res = (res * base) % mod;
		}

		exponent >>= 1;
		base = (base * base) % mod;
	}
	return res;
}
uint64_t power(uint64_t base, uint64_t exponent)
{
	if (exponent == 0) {
		return 1;
	} else if (exponent == 1) {
		return base;
	} else if (exponent % 2 == 0) {
		return power(base * base, exponent / 2);

	} else {
		return base * power(base * base, exponent / 2);
	}
}

static size_t find_start(uint64_t *start, uint64_t subject, uint64_t mod)
{
	double subject_log = log2((double)subject);
	double mod_log = log2((double) mod) / subject_log;
	size_t ret = floor(mod_log);
	*start = power(subject, ret);
	return ret;
}

static uint64_t transform(uint64_t start, uint64_t subject, size_t n, uint64_t mod)
{
	if (n == 1) {
		return (start * subject) % mod;
	} else {
		return start * powermod(subject, n, mod);
	}
}

static size_t find_loop_size(uint64_t subject, uint64_t pubkey, uint64_t mod)
{
	uint64_t key = 1;
	//size_t ret = find_start(&key, subject, mod);
	size_t ret = 0;


	while (key != pubkey) {
		ret += 1;
		key = transform(key, subject, 1, 20201227);
	}

	return ret;
}

static result *part1(input *inp)
{
	uint64_t tobreak, totransform;
	if (inp->card_key > inp->door_key) {
		tobreak = inp->card_key;
		totransform = inp->door_key;
	} else {
		tobreak = inp->door_key;
		totransform = inp->card_key;
	}

	size_t loop_size = find_loop_size(7, tobreak, 20201227);
	uint64_t res = transform(1, totransform, loop_size, 20201227);

	return_result(res);
}

static result *part2(input *inp)
{
	notimpl();
}


day day25 = {
	getinput,
	part1,
	part2,
	freeinput,
};