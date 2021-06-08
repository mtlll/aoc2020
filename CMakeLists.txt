cmake_minimum_required(VERSION 3.13)
project(aoc C)

set(CMAKE_C_STANDARD 11)

add_executable(aoc main.c util/htable.c util/htable.h day4.c day3.c day2.c day1.c day5.c util/io.c util/io.h day6.c day8.c day9.c util/vec.c util/vec.h day11.c day12.c day7.c day10.c flags.h day.h day.c day13.c day14.c day15.c day18.c day16.c day17.c day19.c day20.c day21.c day22.c day23.c util/list.c util/list.h day24.c day25.c util/ndarray.c util/ndarray.h)
target_link_libraries(aoc m)