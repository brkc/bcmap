#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bcmap.h"

static int
print(const void *key0, const void *val0, void *unused)
{
    assert(unused == NULL);
    printf("{%s: %d}\n", (char *) key0, *((int *) val0));
    return 1;
}

static char *
strrand(size_t size)
{
    static char cs[] = "abcdefghijklmnopqrstuvwxyz";
    static size_t cslen = sizeof(cs) - 1;
    char *str = malloc((size + 1) * sizeof(*str));
    size_t i;

    for (i = 0; i < size; i++) {
        size_t n = rand() % cslen;
        str[i] = cs[n];
    }
    str[size] = '\0';
    return str;
}


int
main(int argc, char *argv[])
{
    bcmap *table;
    size_t i;
    char *endptr;
    unsigned long randstrlen;
    unsigned long randstrcount;

    if (argc < 2) {
        fprintf(stderr, "usage: %s RANDSTRLEN RANDSTRCOUNT\n", argv[0]);
        return EXIT_FAILURE;
    }

    randstrlen = strtoul(argv[1], &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "usage: %s RANDSTRLEN RANDSTRCOUNT\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argc < 3) {
        fprintf(stderr, "usage: %s RANDSTRLEN RANDSTRCOUNT\n", argv[0]);
        return EXIT_FAILURE;
    }

    randstrcount = strtoul(argv[2], &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "usage: %s RANDSTRLEN RANDSTRCOUNT\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argc > 3) {
        fprintf(stderr, "usage: %s RANDSTRLEN RANDSTRCOUNT\n", argv[0]);
        return EXIT_FAILURE;
    }

    table = bcmap_new(bcmap_strcmp);
    srand(time(NULL));

    for (i = 0; i < randstrcount; i++) {
        char *key = strrand(randstrlen);
        int *val = malloc(sizeof(*val));
        *val = rand() % 90 + 10;
        bcmap_put(table, key, val);
    }

    bcmap_for(table, print, NULL);
    bcmap_del(table, free, free);
    return EXIT_SUCCESS;
}
