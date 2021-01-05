#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "util/io.h"
#include "util/htable.h"

static size_t getinput(char *filename, uint64_t **nums)
{
    FILE *f = fopen_in_path("../inputs/day9/", filename, "r");

    char *s = fslurp(f, "\r");

    fclose(f);

    char *w = s;
    char *tok;
    size_t bufsize = 128;
    uint64_t *res = calloc(bufsize, sizeof(uint64_t));
    size_t i = 0;

    for (tok = strsep(&w, "\n"); *tok; tok = strsep(&w, "\n")) {
        if (i == bufsize) {
            bufsize <<= 1;
            res = reallocarray(res, bufsize, sizeof(uint64_t));
        }

        sscanf(tok, "%lu", res + i);
        i += 1;
    }

    if (i < bufsize) {
        res = reallocarray(res, i, sizeof(uint64_t));
    }

    *nums = res;
    return i;
}
static size_t u64hash(const void *in, size_t seed)
{
    return *((uint64_t *)in) ^ seed;
}

static bool u64cmp(const void *a, const void *b)
{
    return *((uint64_t *)a) == *((uint64_t *)b) ? true : false;
}

static uint64_t part1(size_t n, size_t npreamb, uint64_t *nums)
{
    uint64_t *preamb = calloc(npreamb, sizeof (uint64_t));
    htable_t *ht = ht_create(&u64hash, &u64cmp, NULL);


    size_t i, j, k;
    uint64_t key, v;

    for (i = 0; i < npreamb; i += 1) {
        preamb[i] = nums[i];
        ht_insert(ht, preamb + i, NULL);
    }

    for (; i < n; i += 1) {
        k = i % npreamb;
        v = nums[i];

        for (j = 0; j < npreamb; j += 1) {
            key = preamb[j];
            key = (key < v) ? v - key : key - v;
            if (ht_get(ht, &key, NULL)) {
                break;
            }
        }

        if (j < npreamb) {
            ht_remove(ht, preamb + k);
            preamb[k] = v;
            ht_insert(ht, preamb + k, NULL);
        } else {
            return v;
        }
    }

    ht_destroy(ht);
    free(preamb);
    return 0;
}
static uint64_t part2(size_t n, uint64_t target, uint64_t *nums)
{
    size_t head, tail;
    uint64_t diff = target;

    for (head = tail = 0; tail < n;) {
        if (diff == nums[tail]) {
            break;
        } else if (diff < nums[tail]) {
            diff += nums[head++];
        } else {
            diff -= nums[tail++];
        }
    }

    uint64_t min, max;
    size_t i;
    min = max = nums[head];

    for (i = head; i < tail; i += 1) {
        if (nums[i] < min) {
            min = nums[i];
        } else if (nums[i] > max) {
            max = nums[i];
        }
    }

    return min + max;
}

void day9()
{
    uint64_t *nums;
    size_t n = getinput("input", &nums);
    uint64_t part1res = part1(n, 25, nums);

    printf("\tPart 1 input: %lu\n", part1res);
    printf("\tPart 2 input: %lu\n", part2(n, part1res, nums));

    free(nums);
}