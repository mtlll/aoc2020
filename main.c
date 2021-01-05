#include <stdio.h>
#include <stdlib.h>

#include "days.h"

static void run_day(int n);

int main(int argc, char **argv)
{
    init_days();
    days[d9]();
    if (argc < 2) {
        printf("Please input days to run. E.g %s 1 2 3", argv[0]);
        return 0;
    }


    int i, day;

    for (i = 1; i < argc; i += 1) {
        day = atoi(argv[i]);
        printf("Day %d:", day);
        run_day(day);
    }

    return 0;
}

static void run_day(int n)
{
    size_t idx = n - 1;

    if (days[idx] != NULL) {
        putchar('\n');
        (*days[idx])();
    } else {
        printf(" No solution for this day yet. \n");
    }

}
