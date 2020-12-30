#include "util/io.h"
#include <stdlib.h>
#include <assert.h>

struct seat {
    char *s;
    int row;
    int column;
    int seatID;
};

static int seatIDcmp(const void *a, const void *b)
{
    struct seat *aa = (struct seat *)a;
    struct seat *bb = (struct seat *)b;

    return aa->seatID - bb->seatID;
}

static void compute_seat(char *seat, int *row, int *column)
{
    int rowmin = 0;
    int rowmax = 127;
    int rowjump = 64;
    int colmin = 0;
    int colmax = 7;
    int coljump = 4;


    while (*seat) {
        switch (*seat) {
            case 'F':
                rowmax -= rowjump;
                rowjump >>= 1;
                break;
            case 'B':
                rowmin += rowjump;
                rowjump >>= 1;
                break;
            case 'L':
                colmax -= coljump;
                coljump >>= 1;
                break;
            case 'R':
                colmin += coljump;
                coljump >>= 1;
                break;
        }
        seat += 1;
    }

    assert(rowmax == rowmin);
    assert(colmax == colmin);

    *row = rowmax;
    *column = colmax;
}

static int seat_ID(int row, int column)
{
    return row * 8 + column;
}

static size_t getinput(char *filename, struct seat **res)
{
    FILE *f = fopen_in_path("../inputs/day5/", filename, "r");

    char **lines;
    size_t nlines = get_lines(f, &lines);
    size_t i;

    fclose(f);

    struct seat *ret = (struct seat *)calloc(nlines, sizeof (struct seat));

    for (i = 0; i < nlines; i += 1) {
        ret[i].s = lines[i];
        compute_seat(ret[i].s, &ret[i].row, &ret[i].column);
        ret[i].seatID = seat_ID(ret[i].row, ret[i].column);
    }

    free(lines);

    qsort(ret, nlines, sizeof (struct seat), seatIDcmp);

    *res = ret;
    return nlines;
}
static void printseat(struct seat *seat)
{
    printf("%s: row %d, column %d, seatID %d\n", seat->s, seat->row, seat->column, seat->seatID);
}
static int part1(char *filename)
{
    struct seat *seats;
    size_t nseats = getinput(filename, &seats);


    for (size_t i = 0; i < nseats; i += 1) {
        printseat(&seats[i]);
    }

    return seats[nseats - 1].seatID;
}

static int part2(char *filename)
{

    struct seat *seats;
    size_t nseats = getinput(filename, &seats);
    struct seat *cur;
    int lastID = seats[0].seatID;

    for (size_t i = 1; i < nseats; i += 1) {
        cur = &seats[i];
        if (cur->seatID - lastID == 2) {
            return lastID + 1;
        }
        lastID = cur->seatID;
    }
}

void day5()
{
    printf("\tPart 1 example: %d", part1("example"));
    printf("\tPart 1 puzzle input: %d", part1("input"));
    printf("\tPart 2 puzzle input: %d", part2("input"));
}

