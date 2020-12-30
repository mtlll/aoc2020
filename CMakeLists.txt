cmake_minimum_required(VERSION 3.13)
project(aoc C)

set(CMAKE_C_STANDARD 99)

add_executable(aoc main.c util/htable.c util/htable.h day4.c day3.c day2.c day1.c days.h day5.c util/io.c util/io.h)