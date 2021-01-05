#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/io.h"
#include "util/htable.h"

static char *get_input(char *filename)
{
    FILE *f = fopen_in_path("../inputs/day6/", filename, "r");

    //fuck Windows
    char *s = fslurp(f, "\r");

    fclose(f);
    return s;
}

static int part1(char *input)
{
    char *seen = calloc(28, sizeof (char));
    int count = 0;
    int res = 0;

    char **w = &input;
    char *tok;

    while ((tok = strsep(w, "\n"))) {
        if (*tok == '\0') {
           res += count;
           bzero(seen, count);
           count = 0;
           continue;
        }

        for (tok += strspn(tok, seen); *tok != '\0'; tok += strspn(tok, seen)) {
            seen[count++] = *tok;
        }
    }

    return res;
}

static void swap(char *a, char *b) {
    if (a != b) {
        char tmp = *a;
        *a = *b;
        *b = tmp;
    }
}

static int part2(char *input)
{
    char *seen = NULL;
    int res = 0;

    char **w = &input;
    char *tok;

    while ((tok = strsep(w,"\n"))) {
        if (*tok == '\0') {
            res += strlen(seen);
            seen = NULL;
            continue;
        } else if (seen == NULL) {
            seen = tok;
            continue;
        }

        for (char *s = seen; *s; s += 1) {
            if (!strchr(tok, *s)) {
                swap(seen++, s);
            }
        }
    }

    return res;
}
void day6()
{
    char *s = get_input("input");

    printf("\tPart 2 input: %d\n", part2(s));

    free(s);
}

