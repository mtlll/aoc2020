#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/io.h"

typedef enum {
    nul, nop, acc, jmp
} opcode ;

static char *opnames[] = {"nul", "nop", "acc", "jmp"};

typedef struct {
    opcode op;
    int argument;
    int accum;
    bool seen;
} instruction;

static void printi(size_t ip, instruction *p)
{
    printf("%lu: %s %d\n", ip, opnames[p->op], p->argument);
}
opcode parse_opcode(char *s) {
    if (strcmp(s, "nop") == 0) {
        return nop;
    } else if (strcmp(s, "acc") == 0) {
        return acc;
    } else if (strcmp(s, "jmp") == 0) {
        return jmp;
    }
}

static size_t getinput(char *filename, instruction **program)
{
    FILE *f = fopen_in_path("../inputs/day8/", filename, "r");
    char *s = fslurp(f, "\r");
    fclose(f);
    char *w = s;
    char *tok;
    size_t bufsize = 128;
    size_t i = 0;
    char opname[4];

    instruction *ret = calloc(bufsize + 1, sizeof (instruction));

    while (*(tok = strsep(&w, "\n"))) {
        if (i == bufsize) {
            bufsize <<= 1;
            ret = reallocarray(ret, bufsize + 1, sizeof (instruction));
        }

        sscanf(tok, "%3s %d", opname, &ret[i].argument);
        ret[i].op = parse_opcode(opname);
        ret[i].seen = false;
        i += 1;
    }

    if (i < bufsize) {
        ret = reallocarray(ret, i + 1, sizeof (instruction));
    }
    ret[i].op = nul;
    ret[i].seen = false;

    free(s);

    *program = ret;
    return i + 1;
}

static bool exec_op(opcode op, int arg, int *accum, size_t *ip)
{
           switch (op) {
            case acc:
                *accum += arg;
            case nop:
                *ip += 1;
                break;
            case jmp:
                *ip += arg;
                break;
            case nul:
                return true;
        }

        return false;
}

static int part1(size_t n, instruction *program)
{
    int accum = 0;
    size_t ip = 0;
    instruction *p;

    for (;;) {
        p = program + ip;

        if (p->seen) {
            return accum;
        } else {
            p->accum = accum;
            p->seen = true;
        }

        exec_op(p->op, p->argument, &accum, &ip);
    }

}
static int part2(size_t n, instruction *program)
{
    int accum = 0;
    size_t ip = 0;

    size_t *to_try = calloc(n, sizeof (size_t));

    size_t ntry;
    size_t i;
    instruction *p;

    /* Enumerate all the instructions we need to try to flip.
     * Since we can only flip one instruction,
     * it only makes sense to try the ones executed in part 1.
     * They've been tagged for us already by part 1 */
    for (i = 0, ntry = 0; i < n; i += 1) {
        p = &program[i];

        if (p->seen && (p->op == nop ||p->op == jmp)) {
            to_try[ntry] = i;
            ntry += 1;
        }
    }

    for (i = 0; i < ntry; i += 1) {
        ip = to_try[i];
        p = program + ip;
        accum = p->accum;
        if (exec_op(p->op == jmp ? nop : jmp, p->argument, &accum, &ip)){
            return accum;
        }

        for (;;) {
            if (ip > n || (p = program + ip)->seen) {
                break;
            } else {
                p->seen = true;
                p->accum = accum;
            }

            if (exec_op(p->op, p->argument, &accum, &ip)) {
                return accum;
            }
        }
    }

    return -1;
}

void day8()
{
    instruction *program;
    size_t ninstructions = getinput("example", &program);
    printf("\tPart 1 example: %d\n", part1(ninstructions, program));
    printf("\tPart 2 example: %d\n", part2(ninstructions, program));

    free(program);

    ninstructions = getinput("input", &program);
    printf("\tPart 1 input: %d\n", part1(ninstructions, program));
    printf("\tPart 2 input: %d\n", part2(ninstructions, program));

    free(program);
}
