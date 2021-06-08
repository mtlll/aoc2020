#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "day.h"
#include "util/io.h"
#include "util/vec.h"


typedef struct {
    size_t idx;
    bool is_recursive;
    ssize_t sublist[4];
    bool has_or;
    ssize_t orlist[4];
    char c;
} rule;

static rule *make_rule()
{
	rule *ret = calloc(1, sizeof (rule));
	for (size_t i = 0; i < 4; i += 1) {
		ret->sublist[i] = -1;
		ret->orlist[i] = -1;
	}

	return ret;
}

static rule *parse_rule(char *s)
{
	rule *r = make_rule();

	r->idx = strtoul(s, &s, 10);
	s += 2;

	if (*s == '"') {
		r->c = *++s;
		return r;
	}

	ssize_t *sl = r->sublist;
	char *tok;

	for (size_t i = 0; (tok = strsep(&s, " "));) {
		if (*tok == '|') {
			r->has_or = true;
			sl = r->orlist;
			i = 0;
			continue;
		}
		sl[i] = strtol(tok, NULL, 10);
		if ((size_t)sl[i] == r->idx) {
			r->is_recursive = true;
		}
		i += 1;
	}
	return r;
}

static int rulecmp(const void *a, const void *b)
{
	const rule *r1 = *((rule **)a);
	const rule *r2 = *((rule **)b);

	if (r1->idx < r2->idx) {
		return -1;
	} else if (r1->idx > r2->idx) {
		return 1;
	} else {
		return 0;
	}
}

typedef struct {
    vec_t *rules;
    vec_t *messages;
} input;

static input *make_input()
{
	input *ret = malloc(sizeof (input));
	vec_callbacks rcbs = {NULL, free};
	ret->rules = vec_create(&rcbs);

	vec_callbacks mcbs = {strdup, free};
	ret->messages = vec_create(&mcbs);

	return ret;
}

static void freeinput(void *in)
{
	input *inp = in;

	vec_destroy(inp->rules);
	vec_destroy(inp->messages);
	free(in);
}



static void *getinput(char *filename)
{
	input *ret = make_input();
	FILE *f = fopen_in_path("../inputs/day19", filename, "r");

	char *s = fslurp(f, "\r");

	fclose(f);

	rule *r;

	char *w = s;
	char *tok, *tok2;
	ssize_t *sl;
	vec_t *rules = ret->rules;

	for (; (tok = strsep(&w, "\n")) && *tok; vec_push(rules, r)) {
		r = parse_rule(tok);
	}

	vec_qsort(rules, rulecmp);

	while ((tok = strsep(&w, "\n"))) {
		vec_push(ret->messages, tok);
	}

	free(s);
	return (void *)ret;
}

static bool message_matches_rule(char **m, rule *r, vec_t *rules);

static bool message_matches_sublist(char **m, ssize_t *sl, vec_t *rules)
{
	char *w = *m;
	rule *r;

	for (size_t i = 0; i < 4 && sl[i] != -1; i += 1) {
		r = vec_get(rules, sl[i]);
		if (!message_matches_rule(&w, r, rules)) {
			return false;
		} else if (r->is_recursive) {
			printf("match: %s remainder %s\n", *m, w);
		}
	}

	*m = w;

	return true;
}
static bool message_matches_rule(char **m, rule *r, vec_t *rules)
{
	if (r->c) {
		if (**m == r->c) {
			*m += 1;
			return true;
		} else {
			return false;
		}
	} else if (message_matches_sublist(m, r->sublist, rules)) {
		return true;
	} else if (r->has_or && message_matches_sublist(m, r->orlist, rules)) {
		return true;
	} else {
		return false;
	}
}
static bool message_matches(char *m, vec_t *rules)
{
	if (message_matches_rule(&m, vec_get(rules, 0), rules) && !*m) {
		return true;
	} else {
		return false;
	}
}

static uint64_t count_matches(input *inp)
{
	uint64_t res = 0;
	char *m;
	size_t nmsg = vec_len(inp->messages);

	for (size_t i = 0; i < nmsg; i += 1) {
		m = vec_get(inp->messages, i);
		if (message_matches(m, inp->rules)) {
			res += 1;
		}
	}

	return res;
}

static result *part1(void *data)
{
	input *inp = data;


	return_result(count_matches(inp));
}

static result *part2(void *data)
{
	input *inp = data;
	uint64_t res = 0;
	vec_t *rules = inp->rules;
	rule *r42 = vec_get(rules, 42);
	rule *r31 = vec_get(rules, 31);
	size_t nmsg = vec_len(inp->messages);
	size_t n42;
	size_t n31;
	char *m;
	for (size_t i = 0; i < nmsg; i += 1) {
		m = vec_get(inp->messages, i);
		if (strlen(m) % 8 != 0) {
			continue;
		}

		for (n42 = 0; message_matches_rule(&m, r42, rules); n42 += 1);
		if (!n42) {
			continue;
		}
		for (n31 = 0; *m && message_matches_rule(&m, r31, rules); n31 += 1);

		if (!*m  && n31 > 0 && n42 > n31) {
			res += 1;
		}
	}

	return_result(res);


}

day day19 = {
	getinput,
	part1,
	part2,
	freeinput,
};