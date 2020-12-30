#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"

FILE *fopen_in_path(const char *path, const char *filename, const char *modes)
{
    size_t pathlen = strlen(path);
    //add 2 to the size to ensure room for adding a / between the path and the filename
    char *fullpath = (char *)calloc(pathlen + strlen(filename) + 2, sizeof (char));

    strcpy(fullpath, path);

    /* ensure trailing slash. No need to fix the null byte since the buffer is zero filled by calloc. */
    if (pathlen > 0 && fullpath[pathlen - 1] != '/') {
        fullpath[pathlen] = '/';
    }

    strcat(fullpath, filename);

    FILE *f = fopen(fullpath, modes);

    free(fullpath);

    return f;
}
/* Count the number of lines and the widest line in the file */

size_t count(FILE *f, size_t *maxwidth)
{
    if (f == NULL) {
        return 0;
    }

    size_t ret = 0;
    size_t maxw = 0;
    size_t width = 0;
    int c;

    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            ret += 1;

            if (width > maxw) {
                maxw = width;
            }
            width = 0;
        } else if (c != '\r'){
            width += 1;
        }
    }

    rewind(f);
    *maxwidth = maxw;
    return ret;
}

size_t get_lines(FILE *f, char ***res)
{
    size_t maxw;
    size_t nlines = count(f, &maxw);
    size_t i = 0;
    size_t j = 0;
    int c;

    char *buf = (char *)calloc(maxw + 1, sizeof (char));

    char **ret = (char **)calloc(nlines, sizeof (char *));

    while ((c = fgetc(f)) != EOF) {
        if (c == '\r') {
            continue;
        } else if (c == '\n') {
            buf[j] = '\0';
            ret[i] = strdup(buf);
            i += 1;
            j = 0;
        } else {
            buf[j++] = c;
        }
    }

    rewind(f);
    free(buf);

    *res = ret;
    return nlines;
}