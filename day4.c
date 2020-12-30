#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

struct passport {
    char *fields[8];
    size_t present;
    size_t valid;
};

typedef enum {byr, iyr, eyr, hgt, hcl, ecl, pid, cid} field;
static char fieldnames[8][4] = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid", "cid"};

static field stofield (const char *s)
{
    field ret;

    for (ret = byr; strcmp(s, fieldnames[ret]) != 0; ret += 1)
        ;

    return ret;
}

static bool byr_valid(char *s) {
    int year = atoi(s);

    if (year >= 1920 && year <= 2002) {
        return true;
    } else {
        return false;
    }
}

static bool iyr_valid(char *s) {
    int year = atoi(s);

    if (year >= 2010 && year <= 2020) {
        return true;
    } else {
        return false;
    }
}

static bool eyr_valid(char *s) {
    int year = atoi(s);

    if (year >= 2020 && year <= 2030) {
        return true;
    } else {
        return false;
    }
}

static bool hgt_valid(char *s) {
    int height;
    char unit[3];

    sscanf(s, "%d%2s", &height, unit);

    if (strcmp(unit, "cm") == 0 && height >= 150 && height <= 193) {
        return true;
    } else if (strcmp(unit, "in") == 0 && height >=59 && height <= 76) {
        return true;
    } else {
        return false;
    }
}

static bool hcl_valid(char *s) {
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

static bool ecl_valid(char *s) {
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

static bool (*validate[7])(char *) = {byr_valid, iyr_valid, eyr_valid, hgt_valid, hcl_valid, ecl_valid, pid_valid};

static void print_passport(struct passport *p)
{
    field i;

    for (i = byr; i <= cid; i += 1) {
        if (p->fields[i] != NULL) {
            printf("%s:%s ", fieldnames[i], p->fields[i]);
        }
    }

    printf("present:%d, valid:%d", p->present, p->valid);
}

typedef enum {fieldname, fieldval, whitespace, cont, newline} state;

static size_t getinput(const char *filename, struct passport **pp)
{
    char path[32] = "../inputs/day4/";
    strcat(path, filename);
    FILE *f = fopen(path, "r");

    if (f == NULL) {
        *pp = NULL;
        return 0;
    }

    size_t npp = 0;
    int c = 0;
    state s = cont;
    while ((c = fgetc(f)) != EOF) {
        switch (s) {
            case cont:
                if (c == '\n') {
                    s = newline;
                }
                break;
            case newline:
                if (c == '\n') {
                    npp += 1;
                    s = cont;
                } else if (c != '\r') {
                    s = cont;
                }

        }
    }

    rewind(f);

    struct passport *res = (struct passport *)calloc(npp, sizeof (struct passport));
    struct passport *cur = res;
    char head[4];
    head[3] = '\0';
    size_t i = 0;
    size_t j = 0;
    field fi;

    char *buf = (char *)calloc(64, sizeof (char));
    s = fieldname;

    while ((c = fgetc(f)) != EOF) {
        switch(s) {
            case fieldname:
                if (c == ':') {
                    fi = stofield(head);
                    i = 0;
                    s = fieldval;
                } else {
                    head[i++] = c;
                }
                break;
            case fieldval:
                if (!isspace(c)) {
                    buf[j++] = c;
                    break;
                } else {
                    buf[j] = '\0';
                    cur->fields[fi] = strdup(buf);
                    cur->present += 1;
                    if (fi != cid) {
                        cur->valid += (*validate[fi])(buf);
                    }
                    j = 0;
                }
            case whitespace:
                if (c == '\r') {
                    s = whitespace;
                } else if (c == '\n') {
                    s = newline;
                } else {
                    s = fieldname;
                }
                break;
            case newline:
                if (c == '\r') {
                    continue;
                } else if (c == '\n') {
                    cur += 1;
                } else {
                    head[i++] = c;
                }
                s = fieldname;
                break;

        }
    }

    rewind(f);
    //fclose(f);
    //free(buf);
    *pp = res;
    return npp;
}

static int part1(const char *filename) {
    struct passport *pp;
    size_t npp, i;
    int count = 0;

    npp = getinput(filename, &pp);

    if (npp == 0 || pp == NULL) {
        return 0;
    }

    for (i = 0; i < npp; i += 1) {
        print_passport(&pp[i]);
        if (pp[i].present == 8) {
            printf(" VALID\n");
            count += 1;
        } else if (pp[i].present == 7 && pp[i].fields[cid] == NULL) {
            printf(" VALID\n");
            count += 1;
        } else {
            printf(" INVALID\n");
        }

    }

    return count;
}

static int part2(const char *filename) {
    struct passport *pp;
    size_t npp, i;
    int count = 0;

    npp = getinput(filename, &pp);

    if (npp == 0 || pp == NULL) {
        return 0;
    }

    for (i = 0; i < npp; i += 1) {
        print_passport(&pp[i]);
        if (pp[i].valid == 7) {
            printf(" VALID\n");
            count += 1;
        } else {
            printf(" INVALID\n");
        }

    }

    return count;
}

void day4()
{
    printf("\tPart 1 example: %d\n", part1("example"));
    printf("\tPart 1 puzzle input: %d\n", part1("input"));
    printf("\tPart 2 invalid: %d\n", part2("invalid"));
    printf("\tPart 2 valid: %d\n", part2("valid"));
    printf("\tPart 2 puzzle input: %d\n", part2("input"));
}