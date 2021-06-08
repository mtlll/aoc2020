//
// Created by mtl on 25/01/2021.
//

#ifndef AOC_DAY_H
#define AOC_DAY_H

#include <stdint.h>

struct result;
typedef struct result result;

result *make_result_uint(uint64_t r);
result *make_result_sint(int64_t r);
result *make_result_double(double r);
result *make_result_string(char *r);

#define return_result(x) do { return _Generic((x), unsigned long: make_result_uint, \
					           long int: make_result_sint, \
					           char *: make_result_string, \
					           double: make_result_double)(x); \
			    } while(0)


#define notimpl() return_result("Not implemented.")

typedef void *(*getinputfp)(char *filename);

typedef result *(*partfun)(void *data);

typedef void (*freedata)(void *in);

typedef struct day {
    getinputfp getinput;
    partfun part1;
    partfun part2;
    freedata freedata;
} day;

void init_days();
void run_day(int day);
void run_all();
void run_last();

#endif