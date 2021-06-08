#ifndef AOC_FLAGS_H
#define AOC_FLAGS_H

#include <stdint.h>

typedef enum {
	RUN_ALL		= 1 << 0,
	RUN_LAST	= 1 << 1,
	RUN_EXAMPLES	= 1 << 2,
	PROFILE		= 1 << 3,
	VERBOSE		= 1 << 4,
	TEST		= 1 << 5

} flag;

extern uint8_t flags;

#endif//AOC_FLAGS_H
