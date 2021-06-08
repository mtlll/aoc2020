#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "day.h"
#include "util/io.h"
#include "util/vec.h"
#include "util/htable.h"

typedef struct {
    vec_t *p1;
    vec_t *p2;
} input;

static void freeinput(void *in)
{
	input *inp = in;

	vec_destroy(inp->p1);
	vec_destroy(inp->p2);
	free(inp);
}

static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day22/", filename, "r");
	char *s = fslurp(f, "\r");
	fclose(f);

	input *ret = calloc(1, sizeof (input));
	ret->p1 = vec_create(NULL);
	ret->p2 = vec_create(NULL);

	char *w = s;
	char *tok;
	vec_t *p = ret->p1;
	uint64_t card;

	while ((tok = strsep(&w, "\n"))) {
		if (*tok == '\0') {
			p = ret->p2;
		} else if (isdigit(*tok)) {
			card = strtoul(tok, NULL, 10);
			vec_push(p, u64cpy(&card));
		}
	}
	free(s);

	return (void *)ret;
}

static uint64_t calculate_score(vec_t *winner, bool is_flat)
{
	uint64_t score = 0;
	uint64_t *card;

	if (is_flat) {
		for (size_t i = 0; i < vec_len(winner); i += 1) {
			card = vec_get(winner, i);
			score += *card;
		}
		return score;
	}

	uint64_t f = vec_len(winner);
	for (size_t i = 0; i < vec_len(winner); i += 1) {
		card = vec_get(winner, i);
		score += *card * f;
		f -= 1;
	}

	return score;
}

static result *part1(void *data)
{
	input *inp = data;

	notimpl();
	vec_t *p1 = vec_cpy(inp->p1);
	vec_t *p2 = vec_cpy(inp->p2);
	uint64_t *c1, *c2;
	do {
		c1 = vec_pop_front(p1);
		c2 = vec_pop_front(p2);

		assert(*c1 != *c2);
		if (*c1 > *c2) {
			vec_push(p1, c1);
			vec_push(p1, c2);
		} else {
			vec_push(p2, c2);
			vec_push(p2, c1);

		}

	} while (vec_len(p1) > 0 && vec_len(p2) > 0);

	vec_t *winner = (vec_len(p1) > 0) ? p1 : p2;
	uint64_t score = calculate_score(winner, 0);

	return_result(score);
}

typedef enum {
    P1 = 0,
    P2 = 1,
    UNKNOWN = 2,
} victor;

typedef struct __attribute__((__packed__)) {
    vec_t *hand;
    uint64_t score;
    uint64_t sum;
    size_t len;
} playerstate;

typedef struct {
    playerstate players[2];
    htable_t *seen;
} gamestate;

static size_t hash_gamestate(gamestate *gs, size_t seed)
{
	//return (fnv1a_64(((void *)gs->players), sizeof (playerstate[2]), seed));
	size_t hash = u64hash(&gs->players[P1].score, seed);
	hash = u64hash(&gs->players[P2].score, hash);

	return hash;
}

static gamestate *gscpy(gamestate *gs)
{
	gamestate *ret = calloc(1, sizeof (gamestate));
	ret->players[P1].score = gs->players[P1].score;
	ret->players[P1].len = gs->players[P1].len;

	ret->players[P2].score = gs->players[P2].score;
	ret->players[P2].len = gs->players[P2].len;

	return ret;
}
static void gsfree(gamestate *gs)
{
	vec_destroy(gs->players[P1].hand);
	vec_destroy(gs->players[P2].hand);
	ht_destroy(gs->seen);
	free(gs);
}

static size_t cmp_gamestate(gamestate *gs1, gamestate *gs2)
{
	/*if (gs1->players[P1].len != gs2->players[P1].len) {
		return false;
	} else if (gs1->players[P2].len != gs2->players[P2].len) {
		return false;
	} else */
	if (gs1->players[P1].score != gs2->players[P1].score) {
		return false;
	} else if (gs1->players[P2].score != gs2->players[P2].score) {
		return false;
	} else {
		return true;
	}
}
static vec_t *vec_shallow_cpy(vec_t *vec, size_t start, size_t n)
{
	vec_t *ret = vec_create(NULL);
	size_t end = start + n;
	for (size_t i = start; i < end; i += 1) {
		vec_push(ret, vec_get(vec, i));
	}

	return ret;
}

static gamestate *new_game(vec_t *p1, vec_t *p2)
{
	gamestate *gs = calloc(1, sizeof (gamestate));
	gs->players[P1].hand = p1;
	gs->players[P1].score = calculate_score(p1, false);
	gs->players[P1].sum = calculate_score(p1, true);
	gs->players[P1].len = vec_len(p1);

	gs->players[P2].hand = p2;
	gs->players[P2].score = calculate_score(p2, false);
	gs->players[P2].sum = calculate_score(p2, true);
	gs->players[P2].len = vec_len(p2);


	ht_callbacks cbs = {gscpy, gsfree, NULL, NULL};
	gs->seen = ht_create(hash_gamestate, cmp_gamestate, &cbs);

	return gs;
}

static bool seen_gs(gamestate *gs)
{
	if (ht_get(gs->seen, gs, NULL)) {
		return true;
	} else {
		ht_insert(gs->seen, gs, NULL);
		return false;
	}
}

static bool have_winner(gamestate *gs, victor *v)
{
	if (gs->players[P2].len == 0) {
		*v = P1;
		return true;
	} else if (gs->players[P1].len == 0) {
		*v = P2;
		return true;
	} else if (seen_gs(gs)) {
		*v = P1;
		return true;
	} else {
		return false;
	}
}

static bool need_subgame (gamestate *gs)
{
	uint64_t *c1, *c2;
	c1 = vec_get(gs->players[P1].hand, 0);
	c2 = vec_get(gs->players[P2].hand, 0);

	if (*c1 < gs->players[P1].len && *c2 < gs->players[P2].len) {
		return true;
	} else {
		return false;
	}
}

static gamestate *new_subgame(gamestate *gs)
{
	uint64_t *c1, *c2;
	c1 = vec_get(gs->players[P1].hand, 0);
	c2 = vec_get(gs->players[P2].hand, 0);

	vec_t *p1 = vec_shallow_cpy(gs->players[P1].hand, 1, *c1);
	vec_t *p2 = vec_shallow_cpy(gs->players[P2].hand, 1, *c2);

	return new_game(p1, p2);
}



static void update_gs(gamestate *gs, victor v)
{
	playerstate *winner, *loser;

	if (v == UNKNOWN) {
		uint64_t *c1, *c2;
		c1 = vec_get(gs->players[P1].hand, 0);
		c2 = vec_get(gs->players[P2].hand, 0);
		v = (*c1 > *c2) ? P1 : P2;
	}

	uint64_t *wc, *lc;
	winner = gs->players + v;
	loser = gs->players + ((v + 1) % 2);

	wc = vec_pop_front(winner->hand);
	lc = vec_pop_front(loser->hand);

	loser->score -= *lc * loser->len;
	loser->sum -= *lc;
	loser->len = vec_len(loser->hand);


	vec_push(winner->hand, wc);
	vec_push(winner->hand, lc);
	winner->score -= *wc * winner->len;
	winner->score += winner->sum;
	winner->sum += *lc;
	winner->score += winner->sum;
	winner->len = vec_len(winner->hand);

}

static uint64_t recursive_game(vec_t *p1, vec_t *p2)
{
	gamestate *gs = new_game(vec_cpy(p1), vec_cpy(p2));
	vec_t *gs_stack = vec_create(NULL);
	uint64_t score;
	victor v;
	uint64_t round = 1;



	while (true) {
		if (have_winner(gs, &v)) {
			if (vec_len(gs_stack) == 0) {
				break;
			} else {
				gsfree(gs);
				gs = vec_pop(gs_stack);
				update_gs(gs, v);
			}
		} else if (need_subgame(gs)) {
			vec_push(gs_stack, gs);
			gs = new_subgame(gs);
		} else {
			update_gs(gs, UNKNOWN);
		}

		round += 1;
	}

	ret:
	vec_destroy(gs_stack);
	score = gs->players[v].score;
	gsfree(gs);
	return score;
}

static result *part2(void *data)
{
	input *inp = data;
	vec_t *p1 = vec_cpy(inp->p1);
	vec_t *p2 = vec_cpy(inp->p2);

	uint64_t score = recursive_game(p1, p2);

	return_result(score);
}

day day22 = {
	getinput,
	part1,
	part2,
	freeinput,
};