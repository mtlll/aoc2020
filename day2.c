#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct pwspec {
    int min;
    int max;
    char c;
    char *pw;
};


static size_t getinput(const char *filename, struct pwspec **result)
{
    char path[32] = "../inputs/day2/";
    strcat(path, filename);
    FILE *f = fopen(path, "r");

    if (f == NULL) {
        *result = NULL;
        return -1;
    }

    //count lines
    int c;
    size_t nlines = 0;

    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            nlines += 1;
        }
    }

    rewind(f);

    struct pwspec *res = calloc(nlines, sizeof (struct pwspec));
    struct pwspec *w = res;
    fpos_t curpos;
    size_t len = 0;
    for (; fscanf(f, "\n%d-%d %c: ", &w->min, &w->max, &w->c) != EOF; w += 1, len = 0) {
        fgetpos(f, &curpos);
        while ((c = fgetc(f)) != EOF && c != '\n') {
            len += 1;
        }
        w->pw = malloc(len + 1);
        fsetpos(f, &curpos);
        fread(w->pw, sizeof (char), len, f);
        w->pw[len] = '\0';

    }

    fclose(f);
    *result = res;
    return nlines;
}

static int part1(const char *filename) {

    struct pwspec *entries;
    size_t nentries;

    nentries = getinput(filename, &entries);

    int charcount;
    int validcount = 0;
    int i, j;

    for (int i = 0; i < nentries; i += 1) {
        for (j = charcount = 0; entries[i].pw[j]; j += 1) {
            if (entries[i].pw[j] == entries[i].c) {
                charcount += 1;
            }
        }

        if (charcount >= entries[i].min && charcount <= entries[i].max) {
            validcount += 1;
        }
    }

    return validcount;
}

static int part2(const char *filename) {

    struct pwspec *entries;
    size_t nentries;

    nentries = getinput(filename, &entries);

    int validcount = 0;
    int i;

    for (i = 0; i < nentries; i += 1) {
        printf("%d-%d %c: %s", entries[i].min, entries[i].max, entries[i].c, entries[i].pw);
        /* logical xor */
        if (!(entries[i].pw[entries[i].min - 1] == entries[i].c) != !(entries[i].pw[entries[i].max - 1] == entries[i].c)) {
            validcount += 1;
            printf(" VALID\n");
        } else {
            printf(" INVALID\n");
        }
    }


    return validcount;
}

void day2()
{
    printf("\tpart 1 example: %d\n", part1("example"));
    printf("\tpart 1 puzzle input: %d\n", part1("input"));
    printf("\tpart 2 example: %d\n", part2("example"));
    printf("\tpart 2 puzzle input: %d\n", part2("input"));
}

