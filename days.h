//
// Created by mtl on 12/29/2020.
//

#ifndef AOC_DAYS_H
#define AOC_DAYS_H

void day1();
void day2();
void day3();
void day4();
void day5();
void day6();

void day8();
void day9();

void (*days[25])();

/* not sure if there's a cleaner way to do this in C. Add every day function to the global array of function pointers */
enum day {
    d1,
    d2,
    d3,
    d4,
    d5,
    d6,
    d7,
    d8,
    d9,
    d10,
    d11,
    d12,
    d13,
    d14,
    d15,
    d16,
    d17,
    d18,
    d19,
    d20,
    d21,
    d22,
    d23,
    d24,
    d25,
};

static inline void init_days()
{
    days[d1] = day1;
    days[d2] = day2;
    days[d3] = day3;
    days[d4] = day4;
    days[d5] = day5;
    days[d6] = day6;

    days[d8] = day8;
    days[d9] = day9;
}

#endif //AOC_DAYS_H
