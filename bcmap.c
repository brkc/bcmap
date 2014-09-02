#include "bcmap.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct entry {
    void *key, *val;
};
struct node {
    struct entry *entry;
    struct node *next;
};
struct map {
    size_t capacity, count;
    struct node **nodes;
    bcmap_cmpfunc cmp;
};

static void *
xmalloc(size_t size)
{
    void *s = malloc(size);
    if (!s) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    return s;
}

static void *
xcalloc(size_t n, size_t size)
{
    void *s = calloc(n, size);
    if (!s) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    return s;
}

static size_t
hash(const void *strv)
{
    const unsigned char *str = strv;
    size_t hashval = 5381;
    int c;
    assert(strv);

    while ((c = *str++))
        hashval = (hashval << 5) + hashval + c;

    return hashval;
}

static void
remap(struct map *map, size_t capacity)
{
    size_t i, oldcount = map->count;
    struct node **oldnodes = map->nodes;
    assert(map);

    map->nodes = xcalloc(capacity, sizeof(*map->nodes));
    map->capacity = capacity;
    map->count = 0;

    for (i = 0; oldcount; i++) {
        struct node *node = oldnodes[i];

        while (node) {
            struct node *next = node->next;
            bcmap_put(map, node->entry->key, node->entry->val);
            free(node);
            node = next;
            oldcount--;
        }
    }
    free(oldnodes);
}

void
bcmap_del(struct map *map,
          bcmap_freefunc keyfree, bcmap_freefunc valfree)
{
    size_t i;
    assert(map);

    for (i = 0; map->count; i++) {
        struct node *node = map->nodes[i];
        while (node) {
            struct node *next = node->next;
            if (keyfree)
                keyfree(node->entry->key);
            if (valfree)
                valfree(node->entry->val);
            free(node);
            node = next;
            map->count--;
        }
    }
    free(map->nodes);
    free(map);
}

void *
bcmap_get(struct map *map, const void *key)
{
    struct node *node = map->nodes[hash(key) % map->capacity];
    assert(map && key);

    while (node) {
        if (!map->cmp(key, node->entry->key))
            return node->entry->val;

        node = node->next;
    }

    return NULL;
}

int
bcmap_rm(struct map *map, const void *key,
         bcmap_freefunc keyfree, bcmap_freefunc valfree)
{
    struct node *node = map->nodes[hash(key) % map->capacity];
    assert(map && key);

    while (node) {

        if (!map->cmp(key, node->entry->key)) {
            if (keyfree)
                keyfree(node->entry->key);
            if (valfree)
                valfree(node->entry->val);

            return 1;
        }

        node = node->next;
    }

    return 0;
}

struct map *
bcmap_new(bcmap_cmpfunc cmp)
{
    struct map *map;
    assert(cmp);

    map = xmalloc(sizeof(*map));
    map->capacity = BCMAP_INITIAL_CAPACITY;
    map->count = 0;
    map->nodes = calloc(map->capacity, sizeof(*map->nodes));
    map->cmp = cmp;

    return map;
}

void
bcmap_put(struct map *map, const void *key, const void *val)
{
    size_t index;
    struct node *node;
    assert(map && key && val);

    if ((double) map->count / map->capacity >= BCMAP_LOAD_FACTOR)
        remap(map, map->capacity * BCMAP_GROWTH_FACTOR);

    index = hash(key) % map->capacity;
    node = map->nodes[index];

    while (node) {
        if (!map->cmp(key, node->entry->key)) {
            node->entry->val = (void *) val;
            return;
        }
        if (!node->next) {
            node->next = xmalloc(sizeof(*node->next));
            node = node->next;
            break;
        }
        node = node->next;
    }

    if (!node)
        map->nodes[index] = node = xmalloc(sizeof(*node));

    node->next = NULL;
    node->entry = xmalloc(sizeof(*node->entry));
    node->entry->key = (void *) key;
    node->entry->val = (void *) val;
    map->count++;
}

int
bcmap_strcmp(const void *lstr, const void *rstr)
{
    assert(lstr && rstr);
    return strcmp((char *) lstr, (char *) rstr);
}

int
bcmap_for(struct map *map,
          int (*f)(const void *, const void *, void *), void *s)
{
    size_t i, count = map->count;
    assert(map && f);

    for (i = 0; count; i++) {
        struct node *node = map->nodes[i];

        while (node) {
            if (!f(node->entry->key, node->entry->val, s))
                return 0;
            node = node->next;
            count--;
        }
    }
    return 1;
}

size_t
bcmap_len(struct map *map)
{
    return map->count;
}
