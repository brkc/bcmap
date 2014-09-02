#ifndef BCHASH_H
#define BCHASH_H 1

#include <stddef.h>

static const size_t BCMAP_INITIAL_CAPACITY = 11;
static const double BCMAP_LOAD_FACTOR = 0.7;
static const double BCMAP_GROWTH_FACTOR = 3.0;

typedef struct map bcmap;

typedef int(*bcmap_cmpfunc)(const void *, const void *);
typedef void(*bcmap_freefunc)(void *); 

bcmap *bcmap_new(bcmap_cmpfunc cmpfunc);

void bcmap_del(bcmap *table,
               bcmap_freefunc keyfreefunc,
               bcmap_freefunc valfreefunc);

void *bcmap_get(bcmap *table, const void *key);

void bcmap_put(bcmap *table, const void *key, const void *val);

int bcmap_strcmp(const void *lstr, const void *rstr);

size_t bcmap_hash(const void *key);

int
bcmap_for(struct map *map,
          int (*f)(const void *, const void *, void *), void *s);

size_t bcmap_len(bcmap *table);

#endif /* !BCHASH_H */
