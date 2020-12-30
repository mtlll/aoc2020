#include <stdio.h>
#ifndef AOC_IO_H
#define AOC_IO_H


FILE *fopen_in_path(const char *path, const char *filename, const char *modes);
size_t count(FILE *f, size_t *maxwidth);
size_t get_lines(FILE *f, char ***res);

#endif //AOC_IO_H
