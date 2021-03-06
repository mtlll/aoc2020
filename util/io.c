#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "io.h"
#include "vec.h"

FILE *fopen_in_path(const char *path, const char *filename, const char *modes)
{
	size_t pathlen = strlen(path);
	//add 2 to the size to ensure room for adding a / between the path and the filename
	char *fullpath = (char *) calloc(pathlen + strlen(filename) + 2,
					 sizeof(char));

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

size_t get_lines(FILE *f, char ***res)
{
	char *s = fslurp(f, "\r");

	vec_callbacks cbs;
	cbs.vcpy = (vec_vcpy) strdup;
	cbs.vfree = free;

	vec_t *lines = vec_create(&cbs);

	char *w = s;
	char *tok;
	for (tok = strsep(&w, "\n"); tok && *tok; tok = strsep(&w, "\n")) {
		vec_push(lines, tok);
	}

	free(s);
	return vec_collect(lines, (void ***) res);
}

/* read all of f into newly allocated buffer, discarding characters in discard */
char *fslurp(FILE *f, const char *discard)
{
	if (f == NULL) return NULL;

	size_t bufsize = 256;
	char *buf = calloc(bufsize + 1, sizeof(char));

	if (buf == NULL) return NULL;

	int c;
	size_t i = 0;

	while ((c = fgetc(f)) != EOF) {
		if (discard != NULL && strchr(discard, c)) {
			continue;
		}

		if (i == bufsize) {
			bufsize <<= 1;
			buf = realloc(buf, bufsize + 1);
			if (buf == NULL) return NULL;
		}
		buf[i++] = c;
	}

	assert(i <= bufsize);

	if (i < bufsize) {
		buf = realloc(buf, i + 1);
	}

	buf[i] = '\0';

	return buf;
}
