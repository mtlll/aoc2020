#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "day.h"
#include "util/io.h"
#include "util/vec.h"


typedef struct {
    uint64_t offset;
    uint64_t val;
} field;

typedef struct {
    uint64_t N;
    uint64_t timestamp;
    vec_t *fields;
} input;

void freeinput(void *in)
{
	input *inp = in;
	vec_destroy(inp->fields);
	free(inp);
}

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day13/", filename, "r");
	char *s = fslurp(f, "\r");
	fclose(f);

	input *res = malloc(sizeof (input));
	res->N - 1;
	char *w = s;
	char *tok;

	res->timestamp = strtoul(strsep(&w, "\n"), NULL, 10);

	vec_callbacks cbs = {NULL, free};
	res->fields = vec_create(&cbs);

	field *fi = calloc(1, sizeof (field));
	for (uint64_t offset = 0; (tok = strsep(&w, ",")); offset += 1) {
		if (isdigit(*tok)) {
			fi->val = strtoul(tok, NULL, 10);
			fi->offset = offset;
			res->N *= fi->val;
			vec_push(res->fields, fi);
			fi = calloc(1, sizeof (field));
		}
	}

	free(s);

	return (void *)res;
}

static result *part1(void *data)
{
	input *inp = data;
	if (inp->timestamp == 0) {
		return_result("Not implemented.");
	}

	vec_t *fields = inp->fields;
	uint64_t timestamp = inp->timestamp;
	uint64_t firstbus = UINT64_MAX;
	uint64_t firstbusID;
	uint64_t curbus;
	field *fi;

	for (size_t i = 0; i < vec_len(fields); i += 1) {
		fi = vec_get(fields, i);

		if (timestamp % fi->val == 0) {
			firstbus = timestamp;
			goto ret;
		}

		curbus = (timestamp / fi->val + 1) * fi->val;

		if (curbus < firstbus) {
			firstbus = curbus;
			firstbusID = fi->val;
		}
	}

	ret:

	return_result((firstbus - timestamp) * firstbusID);
}

static int u64revbusIDcmp(const void *a, const void *b)
{
	field *fielda = *((void **)a);
	field *fieldb = *((void **)b);
	uint64_t IDa = fielda->val;
	uint64_t IDb = fieldb->val;

	return (IDa < IDb) ? 1 :
	       (IDa > IDb) ? -1 :
	       0;
}

static inline uint64_t absdiff(uint64_t a, uint64_t b)
{
	return (a > b) ? a - b : b - a;
}
static result *part2fast(void *data)
{
	input *inp = data;
	vec_t *fields = inp->fields;
	uint64_t N = inp->N;
	uint64_t bi, Ni, xi;
	uint64_t res = 0;

	field *cur;

	for (size_t i = 0; i < vec_len(fields); i += 1) {
		cur = vec_get(fields, i);
		bi = cur->val - (cur->offset % cur->val);
		Ni = N / cur->val;
	}
}
static result *part2(void *data)
{
	input *inp = data;
	vec_t *fields = inp->fields;

	field *fi = vec_get(fields, 0);

	uint64_t jump = fi->val;
	size_t i;
	uint64_t res = 0;
	uint64_t cand = jump;

	for (i = 1; i < vec_len(fields); i += 1) {
		field *cur = vec_get(fields, i);
		uint64_t want = cur->val - (cur->offset % cur->val);
		while (cand % cur->val != want) {
			cand += jump;
		}
		jump *= cur->val;

	}
	return_result(cand);
}

day day13 = {
	getinput,
	part1,
	part2,
	freeinput
};