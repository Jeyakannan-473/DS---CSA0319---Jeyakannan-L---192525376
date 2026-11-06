/* hashmap.h - chained hash table int -> int (used for vehicle -> slot) */
#ifndef HASHMAP_H
#define HASHMAP_H
typedef struct HNode { int key, val; struct HNode *next; } HNode;
typedef struct { HNode **b; int nb; int count; } HashMap;

int  hm_init(HashMap *m, int nbuckets);
void hm_free(HashMap *m);
int  hm_put(HashMap *m, int key, int val);   /* 0 ok, -1 duplicate key, -2 no memory */
int  hm_get(const HashMap *m, int key);      /* value, or -1 if absent               */
int  hm_del(HashMap *m, int key);            /* 1 deleted, 0 absent                  */
int  hm_maxchain(const HashMap *m);
#endif
