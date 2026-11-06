#include <stdlib.h>
#include "hashmap.h"

static unsigned hm_hash(int key, int nb)
{
    return ((unsigned)key * 2654435761u) % (unsigned)nb;   /* multiplicative (Knuth) hash */
}

int hm_init(HashMap *m, int nbuckets)
{
    m->b = (HNode **)calloc((size_t)nbuckets, sizeof(HNode *));
    m->nb = nbuckets; m->count = 0;
    return m->b ? 0 : -2;
}

void hm_free(HashMap *m)
{
    for (int i = 0; i < m->nb; i++)
        for (HNode *p = m->b[i]; p; ) { HNode *q = p->next; free(p); p = q; }
    free(m->b); m->b = NULL; m->nb = m->count = 0;
}

int hm_get(const HashMap *m, int key)
{
    for (HNode *p = m->b[hm_hash(key, m->nb)]; p; p = p->next)
        if (p->key == key) return p->val;
    return -1;
}

int hm_put(HashMap *m, int key, int val)
{
    unsigned h = hm_hash(key, m->nb);
    for (HNode *p = m->b[h]; p; p = p->next)
        if (p->key == key) return -1;
    HNode *n = (HNode *)malloc(sizeof(HNode));
    if (!n) return -2;
    n->key = key; n->val = val; n->next = m->b[h]; m->b[h] = n;   /* insert at head: O(1) */
    m->count++;
    return 0;
}

int hm_del(HashMap *m, int key)
{
    HNode **pp = &m->b[hm_hash(key, m->nb)];
    for (; *pp; pp = &(*pp)->next)
        if ((*pp)->key == key) { HNode *d = *pp; *pp = d->next; free(d); m->count--; return 1; }
    return 0;
}

int hm_maxchain(const HashMap *m)
{
    int mx = 0;
    for (int i = 0; i < m->nb; i++) { int c = 0; for (HNode *p = m->b[i]; p; p = p->next) c++; if (c > mx) mx = c; }
    return mx;
}
