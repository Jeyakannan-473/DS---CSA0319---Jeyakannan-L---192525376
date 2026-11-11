#define _POSIX_C_SOURCE 200809L
/* q7_design.c - Task 7: design of a word index (exact look-up, alphabetical
   autocomplete, online insertion) from first principles. Five candidates are
   built from the same pseudo-words, verified against a brute-force reference
   on 1000 words, and then measured at 1000, 100000 and 1000000 words.
     A sorted array + binary search      B binary search tree (random order)
     C hash table with chaining          D 26-way trie
     E first-child / next-sibling trie (sorted sibling lists)               */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "trie.h"

typedef char Word[16];

static unsigned long long rng = 88172645463325252ULL;
static unsigned long long next_rand(void)
{
    rng ^= rng << 13; rng ^= rng >> 7; rng ^= rng << 17;
    return rng;
}

static double now_ns(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec * 1e9 + (double)t.tv_nsec;
}

static void make_word(char *w)
{
    static const char on[] = "bdfgklmnprst", vo[] = "aeiou";
    int syl = 2 + (int)(next_rand() % 3), k = 0;
    for (int i = 0; i < syl; i++) {
        w[k++] = on[next_rand() % 12];
        w[k++] = vo[next_rand() % 5];
    }
    w[k] = '\0';
}

static int cmp_word(const void *a, const void *b) { return strcmp((const char *)a, (const char *)b); }

/* bytes a malloc'd block really occupies with glibc: header 8, 16-byte steps, minimum 32 */
static size_t chunk(size_t req)
{
    size_t c = (req + 8 + 15) & ~(size_t)15;
    return c < 32 ? 32 : c;
}

/* ============ A: sorted array ============ */
typedef struct { Word *a; int n, cap; } Arr;

static int arr_lower(const Arr *s, const char *key, size_t len)
{
    int lo = 0, hi = s->n;
    while (lo < hi) {
        int mid = (lo + hi) / 2;
        if (strncmp(s->a[mid], key, len) < 0) lo = mid + 1; else hi = mid;
    }
    return lo;
}

static void *arr_build(Word *w, int n)
{
    Arr *s = malloc(sizeof *s);
    s->cap = n + 4096; s->n = n;
    s->a = malloc((size_t)s->cap * sizeof(Word));
    memcpy(s->a, w, (size_t)n * sizeof(Word));
    qsort(s->a, (size_t)n, sizeof(Word), cmp_word);
    return s;
}

static int arr_find(void *p, const char *w)
{
    Arr *s = p;
    int lo = 0, hi = s->n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2, c = strcmp(s->a[mid], w);
        if (c == 0) return 1;
        if (c < 0) lo = mid + 1; else hi = mid - 1;
    }
    return 0;
}

static int arr_ac(void *p, const char *pre, int lim, Word *out)
{
    Arr *s = p;
    size_t len = strlen(pre);
    int k = 0;
    for (int i = arr_lower(s, pre, len); i < s->n && k < lim && strncmp(s->a[i], pre, len) == 0; i++) strcpy(out[k++], s->a[i]);
    return k;
}

static void arr_insert(void *p, const char *w)
{
    Arr *s = p;
    int lo = 0, hi = s->n;
    while (lo < hi) { int mid = (lo + hi) / 2; if (strcmp(s->a[mid], w) < 0) lo = mid + 1; else hi = mid; }
    memmove(s->a + lo + 1, s->a + lo, (size_t)(s->n - lo) * sizeof(Word));
    strcpy(s->a[lo], w);
    s->n++;
}

static size_t arr_heap(void *p) { Arr *s = p; return (size_t)s->cap * sizeof(Word); }
static void arr_free(void *p) { Arr *s = p; free(s->a); free(s); }

/* ============ B: binary search tree ============ */
typedef struct BN { Word key; struct BN *l, *r; } BN;
typedef struct { BN *root; size_t nodes; } Bs;

static void bs_insert(void *p, const char *w)
{
    Bs *t = p;
    BN **pp = &t->root;
    while (*pp) {
        int c = strcmp(w, (*pp)->key);
        if (c == 0) return;
        pp = c < 0 ? &(*pp)->l : &(*pp)->r;
    }
    BN *n = calloc(1, sizeof *n);
    strcpy(n->key, w);
    *pp = n;
    t->nodes++;
}

static void *bs_build(Word *w, int n)
{
    Bs *t = calloc(1, sizeof *t);
    for (int i = 0; i < n; i++) bs_insert(t, w[i]);
    return t;
}

static int bs_find(void *p, const char *w)
{
    BN *c = ((Bs *)p)->root;
    while (c) {
        int r = strcmp(w, c->key);
        if (r == 0) return 1;
        c = r < 0 ? c->l : c->r;
    }
    return 0;
}

static int bs_ac(void *p, const char *pre, int lim, Word *out)
{
    BN *stack[512], *c = ((Bs *)p)->root;
    int top = 0, k = 0;
    size_t len = strlen(pre);
    while (c) {                                 /* descend to the first key >= prefix */
        if (strncmp(c->key, pre, len) >= 0) { stack[top++] = c; c = c->l; }
        else c = c->r;
    }
    while (top && k < lim) {
        BN *x = stack[--top];
        if (strncmp(x->key, pre, len) != 0) break;
        strcpy(out[k++], x->key);
        for (c = x->r; c; c = c->l) stack[top++] = c;
    }
    return k;
}

static size_t bs_heap(void *p) { return ((Bs *)p)->nodes * chunk(sizeof(BN)); }

static void bs_free_nodes(BN *n)
{
    while (n) {                                 /* rotate the left child up: no deep recursion */
        if (n->l) { BN *l = n->l; n->l = l->r; l->r = n; n = l; }
        else { BN *r = n->r; free(n); n = r; }
    }
}

static void bs_free(void *p) { bs_free_nodes(((Bs *)p)->root); free(p); }

/* ============ C: hash table, chaining ============ */
typedef struct HN { Word key; struct HN *next; } HN;
typedef struct { HN **b; unsigned mask; size_t nodes; } Hs;

static unsigned djb2(const char *s)
{
    unsigned h = 5381;
    while (*s) h = h * 33 + (unsigned char)*s++;
    return h;
}

static void hs_insert(void *p, const char *w)
{
    Hs *t = p;
    unsigned i = djb2(w) & t->mask;
    for (HN *x = t->b[i]; x; x = x->next) if (strcmp(x->key, w) == 0) return;
    HN *n = malloc(sizeof *n);
    strcpy(n->key, w);
    n->next = t->b[i];
    t->b[i] = n;
    t->nodes++;
}

static void *hs_build(Word *w, int n)
{
    Hs *t = calloc(1, sizeof *t);
    unsigned cap = 1;
    while (cap < (unsigned)n) cap <<= 1;
    t->mask = cap - 1;
    t->b = calloc(cap, sizeof *t->b);
    for (int i = 0; i < n; i++) hs_insert(t, w[i]);
    return t;
}

static int hs_find(void *p, const char *w)
{
    Hs *t = p;
    for (HN *x = t->b[djb2(w) & t->mask]; x; x = x->next) if (strcmp(x->key, w) == 0) return 1;
    return 0;
}

/* a hash table has no order: collect every match, sort, keep the first lim */
static int hs_ac(void *p, const char *pre, int lim, Word *out)
{
    Hs *t = p;
    size_t len = strlen(pre);
    static Word *tmp;
    static size_t cap;
    size_t m = 0;
    if (!tmp) { cap = 4096; tmp = malloc(cap * sizeof(Word)); }
    for (unsigned i = 0; i <= t->mask; i++)
        for (HN *x = t->b[i]; x; x = x->next)
            if (strncmp(x->key, pre, len) == 0) {
                if (m == cap) { cap *= 2; tmp = realloc(tmp, cap * sizeof(Word)); }
                strcpy(tmp[m++], x->key);
            }
    qsort(tmp, m, sizeof(Word), cmp_word);
    int k = (int)(m < (size_t)lim ? m : (size_t)lim);
    for (int i = 0; i < k; i++) strcpy(out[i], tmp[i]);
    return k;
}

static size_t hs_heap(void *p) { Hs *t = p; return t->nodes * chunk(sizeof(HN)) + (size_t)(t->mask + 1) * sizeof(HN *); }

static void hs_free(void *p)
{
    Hs *t = p;
    for (unsigned i = 0; i <= t->mask; i++) for (HN *x = t->b[i]; x;) { HN *nx = x->next; free(x); x = nx; }
    free(t->b); free(t);
}

/* ============ D: 26-way trie (trie.h) ============ */
static void *t26_build(Word *w, int n)
{
    Trie *t = malloc(sizeof *t);
    trie_init(t);
    for (int i = 0; i < n; i++) trie_insert(t, w[i]);
    return t;
}
static void t26_insert(void *p, const char *w) { trie_insert(p, w); }
static int t26_find(void *p, const char *w) { return trie_search(p, w); }
static int t26_ac(void *p, const char *pre, int lim, Word *out)
{
    char tmp[16][TRIE_MAXW];
    int n = trie_autocomplete(p, pre, lim > 16 ? 16 : lim, tmp);
    for (int i = 0; i < n; i++) strcpy(out[i], tmp[i]);
    return n;
}
static size_t t26_heap(void *p) { return ((Trie *)p)->nodes * chunk(sizeof(TNode)); }
static void t26_free(void *p) { trie_free(p); free(p); }

/* ============ E: first-child / next-sibling trie ============ */
typedef struct LN { char ch, end; struct LN *child, *sib; } LN;
typedef struct { LN *first; size_t nodes; } Lc;

static void lc_insert(void *p, const char *w)
{
    Lc *t = p;
    LN **pp = &t->first, *cur = NULL;
    for (; *w; w++) {
        while (*pp && (*pp)->ch < *w) pp = &(*pp)->sib;
        if (!*pp || (*pp)->ch != *w) {
            LN *n = calloc(1, sizeof *n);
            n->ch = *w;
            n->sib = *pp;
            *pp = n;
            t->nodes++;
        }
        cur = *pp;
        pp = &cur->child;
    }
    if (cur) cur->end = 1;
}

static void *lc_build(Word *w, int n)
{
    Lc *t = calloc(1, sizeof *t);
    for (int i = 0; i < n; i++) lc_insert(t, w[i]);
    return t;
}

static LN *lc_walk(Lc *t, const char *w)
{
    LN *cur = NULL, *lvl = t->first;
    for (; *w; w++) {
        LN *x = lvl;
        while (x && x->ch < *w) x = x->sib;
        if (!x || x->ch != *w) return NULL;
        cur = x;
        lvl = x->child;
    }
    return cur;
}

static int lc_find(void *p, const char *w) { LN *n = lc_walk(p, w); return n && n->end; }

static void lc_collect(LN *n, char *buf, int len, int lim, Word *out, int *found)
{
    for (LN *x = n; x && *found < lim; x = x->sib) {
        buf[len] = x->ch;
        if (x->end) { buf[len + 1] = '\0'; strcpy(out[(*found)++], buf); }
        lc_collect(x->child, buf, len + 1, lim, out, found);
    }
}

static int lc_ac(void *p, const char *pre, int lim, Word *out)
{
    LN *n = lc_walk(p, pre);
    int found = 0;
    char buf[16];
    size_t len = strlen(pre);
    if (!n) return 0;
    memcpy(buf, pre, len);
    if (n->end) strcpy(out[found++], pre);
    lc_collect(n->child, buf, (int)len, lim, out, &found);
    return found;
}

static size_t lc_heap(void *p) { return ((Lc *)p)->nodes * chunk(sizeof(LN)); }

static void lc_free_nodes(LN *n)
{
    while (n) {
        LN *c = n->child, *s = n->sib;
        lc_free_nodes(c);
        free(n);
        n = s;
    }
}

static void lc_free(void *p) { lc_free_nodes(((Lc *)p)->first); free(p); }

/* ============ driver ============ */
typedef struct {
    const char *name;
    void *(*build)(Word *, int);
    int (*find)(void *, const char *);
    int (*ac)(void *, const char *, int, Word *);
    void (*insert)(void *, const char *);
    size_t (*heap)(void *);
    void (*destroy)(void *);
} Impl;

static Impl impls[5] = {
    {"A sorted array", arr_build, arr_find, arr_ac, arr_insert, arr_heap, arr_free},
    {"B BST", bs_build, bs_find, bs_ac, bs_insert, bs_heap, bs_free},
    {"C hash table", hs_build, hs_find, hs_ac, hs_insert, hs_heap, hs_free},
    {"D 26-way trie", t26_build, t26_find, t26_ac, t26_insert, t26_heap, t26_free},
    {"E LCRS trie", lc_build, lc_find, lc_ac, lc_insert, lc_heap, lc_free},
};

static Word *make_set(int n, int extra, Word **extra_out, Word **sorted_out)
{
    int total = n + extra, cnt = total + total / 4 + 16, u = 0;
    Word *all = NULL;
    for (;;) {                                  /* draw more words until enough are distinct */
        free(all);
        all = malloc((size_t)cnt * sizeof(Word));
        for (int i = 0; i < cnt; i++) make_word(all[i]);
        qsort(all, (size_t)cnt, sizeof(Word), cmp_word);
        u = 0;
        for (int i = 0; i < cnt; i++)
            if (i == 0 || strcmp(all[i], all[i - 1]) != 0) {
                if (u != i) memcpy(all[u], all[i], sizeof(Word));   /* never copy a word onto itself */
                u++;
            }
        if (u >= total) break;
        cnt += cnt / 2;
    }
    for (int i = u - 1; i > 0; i--) {                        /* shuffle: random insertion order */
        int j = (int)(next_rand() % (unsigned long long)(i + 1));
        Word tmp; memcpy(tmp, all[i], sizeof tmp); memcpy(all[i], all[j], sizeof tmp); memcpy(all[j], tmp, sizeof tmp);
    }
    Word *w = malloc((size_t)n * sizeof(Word));
    memcpy(w, all, (size_t)n * sizeof(Word));
    *extra_out = malloc((size_t)(extra ? extra : 1) * sizeof(Word));
    memcpy(*extra_out, all + n, (size_t)extra * sizeof(Word));
    *sorted_out = malloc((size_t)n * sizeof(Word));
    memcpy(*sorted_out, w, (size_t)n * sizeof(Word));
    qsort(*sorted_out, (size_t)n, sizeof(Word), cmp_word);
    free(all);
    return w;
}

static int present(Word *sorted, int n, const char *w)
{
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2, c = strcmp(sorted[mid], w);
        if (c == 0) return 1;
        if (c < 0) lo = mid + 1; else hi = mid - 1;
    }
    return 0;
}

/* brute-force reference answer for autocomplete */
static int reference_ac(Word *sorted, int n, const char *pre, int lim, Word *out)
{
    int k = 0;
    size_t len = strlen(pre);
    for (int i = 0; i < n && k < lim; i++) if (strncmp(sorted[i], pre, len) == 0) strcpy(out[k++], sorted[i]);
    return k;
}

static volatile long sink;

int main(void)
{
    /* ---------- verification on 1000 words ---------- */
    Word *extra, *sorted;
    Word *w = make_set(1000, 0, &extra, &sorted);
    int n = 1000;
    printf("VERIFICATION: %d distinct words, 1000 absent words, 600 prefixes (limit 10 results)\n", n);
    Word absent[1000];
    for (int i = 0; i < 1000;) {
        char x[16];
        make_word(x);
        if (!present(sorted, n, x)) strcpy(absent[i++], x);
    }
    char pre[600][8];
    for (int i = 0; i < 600; i++) {
        if (i < 500) {
            const char *src = w[next_rand() % (unsigned long long)n];
            size_t l = 1 + next_rand() % 4;
            memcpy(pre[i], src, l); pre[i][l] = '\0';
        } else {                                              /* prefixes that may match nothing */
            size_t l = 2 + next_rand() % 3;
            for (size_t k = 0; k < l; k++) pre[i][k] = (char)('a' + next_rand() % 26);
            pre[i][l] = '\0';
        }
    }
    printf("%-16s %-14s %-16s %-24s\n", "structure", "words found", "absent rejected", "autocomplete mismatches");
    for (int m = 0; m < 5; m++) {
        void *s = impls[m].build(w, n);
        int found = 0, rej = 0, bad = 0;
        for (int i = 0; i < n; i++) found += impls[m].find(s, w[i]);
        for (int i = 0; i < 1000; i++) rej += !impls[m].find(s, absent[i]);
        for (int i = 0; i < 600; i++) {
            Word got[10], want[10];
            int a = impls[m].ac(s, pre[i], 10, got), b = reference_ac(sorted, n, pre[i], 10, want);
            if (a != b) { bad++; continue; }
            for (int k = 0; k < a; k++) if (strcmp(got[k], want[k]) != 0) { bad++; break; }
        }
        printf("%-16s %-14d %-16d %-24d\n", impls[m].name, found, rej, bad);
        impls[m].destroy(s);
    }
    free(w); free(extra); free(sorted);

    /* ---------- measurements ---------- */
#ifdef SMALL
    int sizes[3] = {1000, 5000, 20000};
#else
    int sizes[3] = {1000, 100000, 1000000};
#endif
    for (int si = 0; si < 3; si++) {
        n = sizes[si];
        rng = 88172645463325252ULL + (unsigned long long)si;
        int ins = 1000;
        w = make_set(n, ins, &extra, &sorted);
        int Q = 200000;
        Word *hit = malloc((size_t)Q * sizeof(Word)), *miss = malloc((size_t)Q * sizeof(Word));
        for (int i = 0; i < Q; i++) strcpy(hit[i], w[next_rand() % (unsigned long long)n]);
        for (int i = 0; i < Q;) {
            char x[16];
            make_word(x);
            if (!present(sorted, n, x)) strcpy(miss[i++], x);
        }
        printf("\nMEASUREMENTS n = %d words\n", n);
        printf("%-16s %-10s %-11s %-10s %-9s %-9s %-11s %-12s\n", "structure", "heap MB", "bytes/word", "build ms",
               "hit ns", "miss ns", "autocmp ns", "insert ns");
        for (int m = 0; m < 5; m++) {
            if (m == 3 && n >= 1000000) {
                printf("%-16s %-10s %-11s %-10s %-9s %-9s %-11s %-12s\n", impls[m].name, "not run", "(memory)", "-", "-", "-", "-", "-");
                continue;
            }
            double t0 = now_ns();
            void *s = impls[m].build(w, n);
            double build = (now_ns() - t0) / 1e6;
            size_t heap = impls[m].heap(s);

            t0 = now_ns();
            long acc = 0;
            for (int i = 0; i < Q; i++) acc += impls[m].find(s, hit[i]);
            double hit_ns = (now_ns() - t0) / Q;
            t0 = now_ns();
            for (int i = 0; i < Q; i++) acc += impls[m].find(s, miss[i]);
            double miss_ns = (now_ns() - t0) / Q;
            sink += acc;

            int AQ = (m == 2) ? (n >= 100000 ? 20 : 200) : 20000;
            Word out[10];
            t0 = now_ns();
            long got = 0;
            for (int i = 0; i < AQ; i++) {
                char p[4];
                memcpy(p, hit[i], 3); p[3] = '\0';
                got += impls[m].ac(s, p, 10, out);
            }
            double ac_ns = (now_ns() - t0) / AQ;
            sink += got;

            t0 = now_ns();
            for (int i = 0; i < ins; i++) impls[m].insert(s, extra[i]);
            double ins_ns = (now_ns() - t0) / ins;
            int ok = 1;
            for (int i = 0; i < ins; i++) if (!impls[m].find(s, extra[i])) ok = 0;
            printf("%-16s %-10.2f %-11.1f %-10.2f %-9.1f %-9.1f %-11.1f %-12.1f%s\n", impls[m].name, (double)heap / 1e6,
                   (double)heap / n, build, hit_ns, miss_ns, ac_ns, ins_ns, ok ? "" : "  INSERT CHECK FAILED");
            impls[m].destroy(s);
        }
        free(w); free(extra); free(sorted); free(hit); free(miss);
    }
    return 0;
}
