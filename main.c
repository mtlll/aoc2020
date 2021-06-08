#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <dirent.h>
#include "day.h"
#include "flags.h"

static void parseargs(int argc, char **argv);
static void print_help(const char *invname);



uint8_t flags;

int main(int argc, char **argv)
{
	init_days();
	parseargs(argc, argv);

	int i, day;
	if (flags & RUN_ALL) {
		run_all();
	} else if (flags & RUN_LAST) {
		run_last();
	} else if (optind == argc) {
		printf("Input a day to run: ");
		scanf("%d", &day);
		run_day(day - 1);
	} else {
		for (i = optind; i < argc; i += 1) {
			day = atoi(argv[i]);
			run_day(day - 1);
		}
	}
	return 0;
}

static void parseargs(int argc, char **argv)
{
	int opt;

	while ((opt = getopt(argc, argv, "alepvt")) != -1) {
		switch(opt) {
			case 'a':
				flags |= RUN_ALL;
				break;
			case 'l':
				flags |= RUN_LAST;
				break;
			case 'e':
				flags |= RUN_EXAMPLES;
				break;
			case 'p':
				flags |= PROFILE;
				break;
			case 'v':
				flags |= VERBOSE;
				break;
			case 't':
				flags |= TEST;
				break;
			default:
				print_help(argv[0]);
				exit(EXIT_FAILURE);
		}
	}
}

static void print_help(const char *invname)
{
	fprintf(stderr, "Usage: %s [-alepvt] days...\n", invname);
}

