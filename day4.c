#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/io.h"
#include "util/vec.h"
#include "day.h"

typedef struct passport {
    char *fields[8];
    size_t present;
    size_t valid;
} passport;

typedef enum {
    byr,
    iyr,
    eyr,
    hgt,
    hcl,
    ecl,
    pid,
    cid
} field;
static char fieldnames[8][4] = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid",
				"cid"};

static field stofield(char *s)
{
	field ret;

	for (ret = byr; strcmp(s, fieldnames[ret]) != 0; ret += 1);

	return ret;
}

static bool byr_valid(char *s)
{
	int year = atoi(s);

	if (year >= 1920 && year <= 2002) {
		return true;
	} else {
		return false;
	}
}

static bool iyr_valid(char *s)
{
	int year = atoi(s);

	if (year >= 2010 && year <= 2020) {
		return true;
	} else {
		return false;
	}
}

static bool eyr_valid(char *s)
{
	int year = atoi(s);

	if (year >= 2020 && year <= 2030) {
		return true;
	} else {
		return false;
	}
}

static bool hgt_valid(char *s)
{
	int height;
	char unit[3];

	sscanf(s, "%d%2s", &height, unit);

	if (strcmp(unit, "cm") == 0 && height >= 150 && height <= 193) {
		return true;
	} else if (strcmp(unit, "in") == 0 && height >= 59 && height <= 76) {
		return true;
	} else {
		return false;
	}
}

static bool hcl_valid(char *s)
{
	int i;

	if (*s++ != '#') {
		return false;
	}

	for (i = 0; i < 6; i += 1) {
		if (!isxdigit(s[i])) {
			return false;
		}
	}

	if (s[i] != '\0') {
		return false;
	}

	return true;
}

static bool ecl_valid(char *s)
{
	if (strlen(s) != 3) {
		return false;
	}

	if (strcmp(s, "amb") == 0) {
		return true;
	} else if (strcmp(s, "blu") == 0) {
		return true;
	} else if (strcmp(s, "brn") == 0) {
		return true;
	} else if (strcmp(s, "gry") == 0) {
		return true;
	} else if (strcmp(s, "grn") == 0) {
		return true;
	} else if (strcmp(s, "hzl") == 0) {
		return true;
	} else if (strcmp(s, "oth") == 0) {
		return true;
	} else {
		return false;
	}
}

static bool pid_valid(char *s)
{
	int i;

	for (i = 0; s[i] != '\0'; i += 1) {
		if (!isdigit(s[i])) {
			return false;
		}
	}

	if (i != 9) {
		return false;
	}

	return true;
}

static bool
(*validate[7])(char *) = {byr_valid, iyr_valid, eyr_valid, hgt_valid, hcl_valid,
			  ecl_valid, pid_valid};

static void print_passport(passport *p)
{
	field i;

	for (i = byr; i <= cid; i += 1) {
		if (p->fields[i] != NULL) {
			printf("%s:%s ", fieldnames[i], p->fields[i]);
		}
	}

	printf("present:%lu, valid:%lu", p->present, p->valid);
}

typedef enum {
    fieldname,
    fieldval,
    whitespace,
    cont,
    newline
} state;

typedef struct {
    size_t npp;
    passport **passports;
} input;

static void freeinput(void *in)
{
	input *inp = in;

	for (size_t i = 0; i < inp->npp; i += 1) {
		for (size_t j = 0; j < 8; j += 1) {
			free(inp->passports[i]->fields[j]);
		}
		free(inp->passports[i]);
	}

	free(inp);

}
static void *getinput(char *filename)
{
	FILE *f = fopen_in_path("../inputs/day4/", filename, "r");
	char *s = fslurp(f, "\r");
	fclose(f);


	char *tok, *w, *fld;
	w = s;

	vec_t *vec = vec_create(NULL);
	passport *p = calloc(1, sizeof (passport));
	field fi;

	while ((tok = strsep(&w, "\n\t "))) {
		if (*tok == '\0') {
			vec_push(vec, (void *)p);
			if (*(s + 1) == '\0') {
				break;
			}
			p = calloc(1, sizeof (passport));
			continue;

		}
		fi = stofield(strsep(&tok, ":"));
		p->fields[fi] = strdup(tok);
		p->present += 1;
		if (fi != cid) {
			p->valid += validate[fi](tok);
		}

	}

	free(s);
	input *res = malloc(sizeof (input));
	res->npp = vec_collect(vec, (void ***)&res->passports);

	return (void *)res;
}


static result *part1(void *data)
{
	input *inp = data;
	size_t npp = inp->npp;
	passport **passports = inp->passports;

	uint64_t count = 0;
	passport *p;

	for (size_t i = 0; i < npp; i += 1) {
		p = passports[i];
		if (p->present == 8) {
			count += 1;
		} else if (p->present == 7 && p->fields[cid] == NULL) {
			count += 1;
		}
	}

	return_result(count);
}

static result *part2(void *data)
{
	input *inp = data;
	size_t npp = inp->npp;
	passport **passports = inp->passports;

	uint64_t count = 0;
	passport *p;

	for (size_t i = 0; i < npp; i += 1) {
		p = passports[i];
		if (p->valid == 7) {
			count += 1;
		}
	}

	return_result(count);
}

day day4 = {
	getinput,
	part1,
	part2,
	freeinput,
};