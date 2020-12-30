#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern void (*days[25])();
static size_t getinput(const char *filename, int **result);

static int part1(const char *filename)
{
    int *entries;
    size_t length;

    int res = 0;

    length = getinput(filename, &entries);

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

static int part2(const char *filename)
{
    int *entries;
    size_t length;

    int res = 0;

    length = getinput(filename, &entries);

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

void day1()
{
    printf("\tPart 1 example: %d\n", part1("example"));
    printf("\tPart 1 puzzle input: %d\n", part1("input"));
    printf("\tPart 2 example: %d\n", part2("example"));
    printf("\tPart 2 puzzle input: %d\n", part2("input"));
}

static size_t getinput(const char *filename, int **result)
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