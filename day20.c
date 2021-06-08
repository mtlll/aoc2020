#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

#include "day.h"
#include "flags.h"
#include "util/io.h"
#include "util/vec.h"

#define SIDELEN 10
#define NSIDES 4

struct tile;
typedef struct tile tile;

typedef struct {
    size_t side;
    tile *t;
} match ;

enum {
    TOP = 0,
    RIGHT = 1,
    BOT = 2,
    LEFT = 3,
};
struct __attribute__((__packed__)) tile {
    uint64_t id;
    char *pixels[SIDELEN];
    union {
        struct {
	    char *top, *right, *bot, *left;
	};
        char *sides[4];
    };
    size_t nmatches;
    match matches[4];
    bool locked;
};


static void print_tile(const tile *t);

typedef struct {
    vec_t *tiles;
    tile *corner;
} input;
static tile *make_tile()
{
	tile *ret = calloc(1, sizeof (tile));
	return ret;
}

static void freetile(tile *t)
{
	for (size_t i = 0; i < SIDELEN; i += 1) {
		free(t->pixels[i]);
	}
	free(t->left);
	free(t->right);
	free(t);
}

static void get_sides(tile *t)
{
	t->top = t->pixels[0];
	t->bot = t->pixels[SIDELEN - 1];
	t->left = calloc(SIDELEN + 1, sizeof (char));
	t->right = calloc(SIDELEN + 1, sizeof (char));

	for (size_t i = 0; i < SIDELEN; i += 1) {
		t->left[i] = t->pixels[i][0];
		t->right[i] = t->pixels[i][SIDELEN - 1];
	}
}
static void freeinput(void *in)
{
	input *inp = in;
	vec_destroy(inp->tiles);
	free(inp);
}


static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day20/", filename, "r");
	char *s = fslurp(f, "\r");
	fclose(f);

	vec_callbacks cbs = {NULL, freetile};
	input *ret = calloc(1, sizeof (input));
	ret->tiles = vec_create(&cbs);


	char *w = s;
	char *tok;
	tile *t;

	while ((tok = strsep(&w, "\n")) && w) {
		t = make_tile();
		sscanf(tok, "Tile %lu", &t->id);

		for (size_t i = 0; (tok = strsep(&w, "\n")) && *tok; i += 1) {
			t->pixels[i] = strdup(tok);
		}
		get_sides(t);

		vec_push(ret->tiles, t);

	}
	if (flags & VERBOSE) {
		t = vec_get(ret->tiles, 0);
		print_tile(t);

	}

	free(s);

	return (void *)ret;
}

static void print_tile(const tile *t)
{
	printf("Tile %lu:\n", t->id);
	for (size_t i = 0; i < SIDELEN; i += 1) {
		printf("%s\n", t->pixels[i]);
	}
	printf("Top: %s\n", t->top);
	printf("Right: %s\n", t->right);
	printf("Bot: %s\n", t->bot);
	printf("Left: %s\n", t->left);
}

int revcmp(char *s1, char *s2) {
	s2 += strlen(s1);
	int ret;

	while (*s1 && !(ret = *s1++ - *--s2));

	return ret;
}
int sides_match(char *s1, char *s2)
{
	if (strcmp(s1, s2) == 0) {
		return 1;
	} else if (revcmp(s1, s2) == 0) {
		return -1;
	} else {
		return 0;
	}
}
void match_tiles(tile *t1, tile *t2)
{
	char **t1s = t1->sides;
	char **t2s = t2->sides;

	for (size_t i = 0; i < NSIDES; i += 1) {
		for (size_t j = 0; j < NSIDES; j += 1) {
			if (sides_match(t1s[i], t2s[j])) {
				t1->matches[t1->nmatches].side = i;
				t1->matches[t1->nmatches++].t = t2;
				t2->matches[t2->nmatches].side = j;
				t2->matches[t2->nmatches++].t = t1;
			}
		}
	}
}

static result *part1(void *data)
{
	uint64_t res = 1;
	input *inp = data;
	vec_t *tiles = inp->tiles;

	size_t ntiles = vec_len(tiles);

	tile *t1, *t2;

	for (size_t i = 0; i < ntiles; i += 1) {
		t1 = vec_get(tiles, i);
		for (size_t j = i + 1; j < ntiles; j += 1) {
			t2 = vec_get(tiles, j);
			match_tiles(t1, t2);
		}

		assert(t1->nmatches > 1);
		if (t1->nmatches == 2) {
			res *= t1->id;
			if (!inp->corner) {
				inp->corner = t1;
			}
		}
	}

	return_result(res);

}

static void revstr(char *s)
{
	size_t n = strlen(s);
	char tmp;
	for (size_t i = 0; i < n/2; i += 1) {
		tmp = s[i];
		s[i] = s[n - i - 1];
		s[n - i - 1] = tmp;
	}
}

static void hflip_2d(char **arr, size_t n)
{
	for (size_t i = 0; i < n; i += 1) {
		revstr(arr[i]);
	}
}

static void rot_2d(char **arr, size_t n)
{
	char tmp;
	for (size_t i = 0; i < n; i += 1) {
		for (size_t j = i; j < n; j += 1) {
			tmp = arr[i][j];
			arr[i][j] = arr[j][i];
			arr[j][i] = tmp;
		}
	}

	hflip_2d(arr, n);
}

void rotate(tile *t)
{
	rot_2d(t->pixels, SIDELEN);
	free(t->left);
	free(t->right);
	get_sides(t);
	for (size_t i = 0; i < t->nmatches; i += 1) {
		t->matches[i].side += 1;
		t->matches[i].side %= NSIDES;
	}
}

static void hflip(tile *t)
{
	hflip_2d(t->pixels, SIDELEN);
	char *tmp;
	tmp = t->left;
	t->left = t->right;
	t->right = tmp;


	for (size_t i = 0; i < t->nmatches; i += 1) {
		if (t->matches[i].side % 2 == 1) {
			t->matches[i].side += 2;
			t->matches[i].side %= NSIDES;
		}
	}
}
static void vflip(tile *t)
{
	char *tmp;
	for (size_t i = 0; i < SIDELEN / 2; i += 1) {
		tmp = t->pixels[i];
		t->pixels[i] = t->pixels[SIDELEN - 1 - i];
		t->pixels[SIDELEN - 1 - i] = tmp;
	}

	tmp = t->top;
	t->top = t->bot;
	t->bot = tmp;
	revstr(t->left);
	revstr(t->right);

	for (size_t i = 0; i < t->nmatches; i += 1) {
		if (t->matches[i].side % 2 == 0) {
			t->matches[i].side += 2;
			t->matches[i].side %= NSIDES;
		}
	}
}

void orient_corner(tile *t)
{
	assert(t->nmatches == 2);
	size_t *s1 = &(t->matches[0].side);
	size_t *s2 = &(t->matches[1].side);
	while (!((*s1 == RIGHT && *s2 == BOT) || (*s2 == RIGHT && *s1 == BOT))) {
		rotate(t);
	}

	t->locked = true;
}
void orient_tile(tile *t, tile *to, size_t side)
{
	size_t want = (side + 2) % NSIDES;
	int cmp;

	while (!(cmp = sides_match(to->sides[side], t->sides[want]))) {
		rotate(t);
	}

	if (cmp == -1) {
		if (want % 2 == 0) {
			hflip(t);
		} else {
			vflip(t);
		}
	}
	t->locked = true;
}
void orient_tiles(tile *t1)
{
	tile *t2;
	for (size_t i = 0; i < t1->nmatches; i += 1) {
		t2 = t1->matches[i].t;
		if (!t2->locked) {
			orient_tile(t2, t1, t1->matches[i].side);
			orient_tiles(t2);
		}
	}
}
tile *find_match(tile *t, size_t side)
{
	tile *ret = NULL;

	for (size_t i = 0; i < t->nmatches; i += 1) {
		if (t->matches[i].side == side) {
			ret = t->matches[i].t;
			break;
		}
	}

	return ret;
}

typedef struct {
    int8_t i, j;
} offset;
#define NOFFSETS 16

offset offsets[NOFFSETS] = {{0, 0},
			    {1, 1},
			    {1, 4},
			    {0, 5},
			    {0, 6},
			    {1, 7},
			    {1, 10},
			    {0, 11},
			    {0, 12},
			    {1, 13},
			    {1, 16},
			    {0, 17},
			    {0, 18},
			    {-1, 18},
			    {0, 19}};

static bool is_mon(char **img, size_t i, size_t j)
{
	offset *p;
	char *c;
	for (size_t k = 0; k < NOFFSETS; k += 1) {
		p = &offsets[k];
		c = &img[i + p->i][j + p->j];
		if (!(*c == '#' || *c == 'O')) {
			return false;
		}
	}

	for (size_t k = 0; k < NOFFSETS; k += 1) {
		p = &offsets[k];
		img[i + p->i][j + p->j] = 'O';
	}

	return true;
}
static uint32_t count_mon(char **img, size_t n)
{
	uint32_t cnt = 0;
	for (size_t i = 1; i < n - 1; i += 1) {
		for (size_t j = 0; j < n - 20; j += 1) {
			if (is_mon(img, i, j)) {
				cnt += 1;
			}
		}
	}

	return cnt;
}

static void find_monsters(char **img, size_t n)
{
	uint32_t cnt = 0;
	size_t i;
	while (!cnt) {
		for(i = 3; i-- && !(cnt = count_mon(img, n)); rot_2d(img, n));
		hflip_2d(img, n);
	}
}

static result *part2(input *inp)
{
	if (flags & VERBOSE) {
		printf("\n");
		print_tile(inp->corner);
		rotate(inp->corner);
		print_tile(inp->corner);
		hflip(inp->corner);
		print_tile(inp->corner);
		vflip(inp->corner);
		print_tile(inp->corner);
	}
	orient_corner(inp->corner);
	orient_tiles(inp->corner);
	size_t ntiles = vec_len(inp->tiles);
	size_t nchunks = (size_t)sqrt((double)ntiles);
	size_t chunklen = SIDELEN - 2;
	size_t slen = chunklen * nchunks;
	char **image = reallocarray(NULL, slen, sizeof (char *));
	char *w;
	size_t i = 0;

	tile *cur = inp->corner;

	do {
		for (size_t j = 1; j < SIDELEN - 1; j += 1) {
			w = image[i] = malloc(slen + 1);
			tile *cur2 = cur;
			do {
				strncpy(w, cur2->pixels[j] + 1, chunklen);
				w += chunklen;
			} while ((cur2 = find_match(cur2, RIGHT)));

			*w = '\0';

			if (flags & VERBOSE) {
				printf("%s\n", image[i]);
			}

			i += 1;
		}
	} while ((cur = find_match(cur, BOT)));

	uint64_t ret = 0;

	if (flags & VERBOSE) {
		printf("\n\n");
	}
	find_monsters(image, slen);
	for (i = 0; i < slen; i += 1) {
		if (flags &VERBOSE) {
			printf("%s\n", image[i]);
		}
		for (size_t j = 0; j < slen; j += 1) {
			if (image[i][j] == '#') {
				ret += 1;
			}
		}
		free(image[i]);
	}

	free(image);

	return_result(ret);
}

day day20 = {
	getinput,
	part1,
	part2,
	freeinput,
};