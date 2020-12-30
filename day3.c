#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TREE '#'

static size_t getinput(const char *filename, char ***map)
{
    char path[32] = "../inputs/day3/";
    strcat(path, filename);
    FILE *f = fopen(path, "r");
    char **res = NULL;
    int i, j;

    if (f == NULL) {
        *map = NULL;
        return -1;
    }

    //count lines
    int c;
    size_t nlines = 0;
    size_t nrows = 0;

    while ((c = fgetc(f)) != '\n' && c != '\r') {
        nrows += 1;
    }

    do {
        if (c == '\n') {
            nlines += 1;
        }
    } while ((c = fgetc(f)) != EOF);

    rewind(f);

    char square;
    res = (char **) calloc(nlines + 1, sizeof (char **));
    for (i = 0; i < nlines; i += 1) {
        res[i] = (char *) calloc(nrows, sizeof (char));
        fread(res[i], sizeof (char), nrows, f);
        //Because of Windows, it's unclear whether the line ending is LF or CRLF,
        //so use fscanf behaviour to read past the whitespace either way
        fscanf(f, "\n");
    }
    res[nlines] = NULL;

    fclose(f);
    *map = res;
    return nrows;
}

static int count_trees(char **map, size_t nlines, size_t nrows, int linestep, int rowstep)
{
    int i, j;
    int res = 0;
    for (i = 0, j = 0; i < nlines; i += linestep, j = (j + rowstep) % nrows) {
        if (map[i][j] == TREE) {
            res += 1;
        }
    }
    return res;
}

static int part1(const char *filename)
{
    char **map;
    size_t nrows;
    size_t nlines = 0;
    int res = 0;

    int i, j;
    nrows = getinput(filename, &map);

    if (map == NULL || nrows == 0) {
        goto ret;
    }

    for (i = 0; map[i] != NULL; i += 1) {
        nlines += 1;
    }

    res = count_trees(map, nlines, nrows, 1, 3);

    ret:
    for (i = 0; map[i] != NULL; i += 1) {
        free(map[i]);
    }

    free(map);

    return res;
}

static long part2 (const char *filename) {
    char **map;
    int nrows;
    int nlines = 0;
    long res = 1; //start at 1 since we're multiplying.
    int i;
    nrows = getinput(filename, &map);

    if (map == NULL || nrows == 0) {
        goto ret;
    }

    for (i = 0; map[i] != NULL; i += 1) {
        nlines += 1;
    }

    res *= count_trees(map, nlines, nrows, 1, 1);
    res *= count_trees(map, nlines, nrows, 1, 3);
    res *= count_trees(map, nlines, nrows, 1, 5);
    res *= count_trees(map, nlines, nrows, 1, 7);
    res *= count_trees(map, nlines, nrows, 2, 1);

    ret:
    for (i = 0; map[i] != NULL; i += 1) {
        free(map[i]);
    }

    free(map);

    return res;
}

void day3()
{
    printf("\tPart 1 example: %d\n", part1("example"));
    printf("\tPart 1 puzzle input: %d\n", part1("input"));
    printf("\tPart 2 example: %ld\n", part2("example"));
    printf("\tPart 2 puzzle input: %ld\n", part2("input"));
}