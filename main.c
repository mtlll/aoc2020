#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int day1part1(const char *filename);
int day1part2(const char *filename);
int day2part1(const char *filename);
int day2part2(const char *filename);

size_t day1getinput(const char *filename, int **result);

int main(int argc, char **argv)
{
    /*
    printf("Day 1:\n");
    printf("\tPart 1 example: %d\n", day1part1("example"));
    printf("\tPart 1 puzzle input: %d\n", day1part1("input"));
    printf("\tPart 2 example: %d\n", day1part2("example"));
    printf("\tPart 2 puzzle input: %d\n", day1part2("input"));
    */

    printf("Day 2:\n");
    printf("\tPart 2 example: %d\n", day2part2("example"));
    printf("\tPart 2 puzzle input: %d\n", day2part2("input"));
    //printf("\tPart 2 example: %d\n", day1part2("example"));
    //printf("\tPart 2 puzzle input: %d\n", day1part2("input"));

}


int day1part1(const char *filename)
{
    int *entries;
    size_t length;

    int res = 0;

    length = day1getinput(filename, &entries);

    if (entries == NULL || length <= 0) {
        printf("well, fuck\n");
        goto ret;
    }

    int i, j;
    for (i = 0; i < length; ++i) {
        for (j = i; j < length; ++j) {
            if (entries[i] + entries[j] == 2020) {
                res = entries[i] * entries[j];
                //printf("res = %d at i = %d, j = %d\n", res, i, j);
                goto ret;
            }
        }
    }
    ret:
    free(entries);
    return res;
}

int day1part2(const char *filename)
{
    int *entries;
    size_t length;

    int res = 0;

    length = day1getinput(filename, &entries);

    if (entries == NULL || length <= 0) {
        printf("well, fuck\n");
        goto ret;
    }

    int i, j, k;
    for (i = 0; i < length; ++i) {
        for (j = i; j < length; ++j) {
            for (k = j; k < length; ++k) {
                if (entries[i] + entries[j] + entries[k] == 2020) {
                    res = entries[i] * entries[j] * entries[k];
                    //printf("res = %d at i = %d, j = %d\n", res, i, j);
                    goto ret;
                }
            }
        }
    }
    ret:
    free(entries);
    return res;
}

struct pwspec {
    int min;
    int max;
    char c;
    char *pw;
};

size_t day2getinput(const char *filename, struct pwspec **result)
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

int day2part1(const char *filename) {

    struct pwspec *entries;
    size_t nentries;

    nentries = day2getinput(filename, &entries);

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

int day2part2(const char *filename) {

    struct pwspec *entries;
    size_t nentries;

    nentries = day2getinput(filename, &entries);

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

size_t day1getinput(const char *filename, int **result)
{
    char path[32] = "../inputs/day1/";
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

    int *res = calloc(nlines, sizeof (int));

    int *w = res;

    rewind(f);

    while (fscanf(f, "\n%d", w) != EOF) {
        w += 1;
    }
    fclose(f);
    *result = res;
    return nlines;
}
