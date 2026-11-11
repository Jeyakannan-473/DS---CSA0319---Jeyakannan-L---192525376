/* splay.h - Task 5: bottom-up splay tree of int keys (no duplicates) with
   parent pointers. Every search, insert and delete ends by splaying a node
   to the root with zig, zig-zig and zig-zag steps, which are counted.
   path = number of nodes visited on the way down (the cost of an access). */
#ifndef SPLAY_H
#define SPLAY_H
#include <stdio.h>
#include <stdlib.h>

typedef struct SNode {
    int key;
    struct SNode *l, *r, *p;
} SNode;

typedef struct {
    SNode *root;
    size_t size;
    long zig, zigzig, zigzag;     /* splay steps by type */
    long rotations;               /* single rotations (zig-zig and zig-zag count two) */
    int trace;                    /* 1 = print every splay step */
} Splay;

static inline void sp_init(Splay *t)
{
    t->root = NULL; t->size = 0;
    t->zig = t->zigzig = t->zigzag = t->rotations = 0;
    t->trace = 0;
}

/* rotate x above its parent (one single rotation) */
static inline void sp_rotate(Splay *t, SNode *x)
{
    SNode *p = x->p, *g = p->p;
    if (x == p->l) {                      /* right rotation about p */
        p->l = x->r;
        if (x->r) x->r->p = p;
        x->r = p;
    } else {                              /* left rotation about p */
        p->r = x->l;
        if (x->l) x->l->p = p;
        x->l = p;
    }
    p->p = x;
    x->p = g;
    if (g) { if (g->l == p) g->l = x; else g->r = x; }
    t->rotations++;
}

static inline void sp_splay(Splay *t, SNode *x)
{
    while (x->p) {
        SNode *p = x->p, *g = p->p;
        if (!g) {
            if (t->trace) printf("    zig     x=%d parent=%d\n", x->key, p->key);
            sp_rotate(t, x);
            t->zig++;
        } else if ((x == p->l) == (p == g->l)) {
            if (t->trace) printf("    zig-zig x=%d parent=%d grandparent=%d\n", x->key, p->key, g->key);
            sp_rotate(t, p);                  /* rotate the parent first */
            sp_rotate(t, x);
            t->zigzig++;
        } else {
            if (t->trace) printf("    zig-zag x=%d parent=%d grandparent=%d\n", x->key, p->key, g->key);
            sp_rotate(t, x);                  /* rotate x twice */
            sp_rotate(t, x);
            t->zigzag++;
        }
    }
    t->root = x;
}

/* returns 1 when the key is present. The last node on the search path is
   splayed to the root whether or not the key was found. */
static inline int sp_access(Splay *t, int k, int *path)
{
    SNode *c = t->root, *last = NULL;
    int len = 0, found = 0;
    while (c) {
        len++;
        last = c;
        if (k == c->key) { found = 1; break; }
        c = k < c->key ? c->l : c->r;
    }
    if (last) sp_splay(t, last);
    if (path) *path = len;
    return found;
}

/* returns 1 when inserted, 0 for a duplicate key (the existing node is splayed) */
static inline int sp_insert(Splay *t, int k)
{
    SNode *c = t->root, *par = NULL;
    while (c) {
        par = c;
        if (k == c->key) { sp_splay(t, c); return 0; }
        c = k < c->key ? c->l : c->r;
    }
    SNode *n = malloc(sizeof *n);
    if (!n) exit(1);
    n->key = k; n->l = n->r = NULL; n->p = par;
    if (!par) t->root = n;
    else if (k < par->key) par->l = n;
    else par->r = n;
    t->size++;
    sp_splay(t, n);
    return 1;
}

/* returns 1 when removed, 0 when absent */
static inline int sp_delete(Splay *t, int k)
{
    if (!t->root || !sp_access(t, k, NULL) || t->root->key != k) return 0;
    SNode *x = t->root, *L = x->l, *R = x->r;
    free(x);
    t->size--;
    if (!L) { t->root = R; if (R) R->p = NULL; return 1; }
    L->p = NULL;
    SNode *m = L;
    while (m->r) m = m->r;
    t->root = L;
    sp_splay(t, m);                       /* the largest key of L becomes its root */
    m->r = R;
    if (R) R->p = m;
    t->root = m;
    return 1;
}

static inline void sp_shape(const SNode *n)
{
    if (!n) { printf("-"); return; }
    if (!n->l && !n->r) { printf("%d", n->key); return; }
    printf("%d(", n->key);
    sp_shape(n->l); printf(","); sp_shape(n->r);
    printf(")");
}

/* level count, iterative */
static inline int sp_levels(const Splay *t)
{
    if (!t->root) return 0;
    SNode **q = malloc(t->size * sizeof *q);
    size_t head = 0, tail = 0;
    int h = 0;
    q[tail++] = t->root;
    while (head < tail) {
        size_t level = tail - head;
        h++;
        while (level--) {
            SNode *x = q[head++];
            if (x->l) q[tail++] = x->l;
            if (x->r) q[tail++] = x->r;
        }
    }
    free(q);
    return h;
}

/* BST order, parent links and node count */
static inline int sp_valid(const Splay *t)
{
    if (!t->root) return t->size == 0;
    if (t->root->p) return 0;
    SNode **stack = malloc((t->size + 1) * sizeof *stack);
    size_t top = 0, count = 0;
    SNode *c = t->root;
    long prev = -2147483649L;
    int ok = 1;
    while (c || top) {
        while (c) { stack[top++] = c; c = c->l; }
        c = stack[--top];
        if ((long)c->key <= prev) ok = 0;
        prev = c->key;
        if (c->l && c->l->p != c) ok = 0;
        if (c->r && c->r->p != c) ok = 0;
        count++;
        c = c->r;
    }
    free(stack);
    return ok && count == t->size;
}

static inline void sp_free(Splay *t)
{
    SNode *c = t->root;
    while (c) {
        if (c->l) { SNode *l = c->l; c->l = l->r; l->r = c; c = l; }
        else { SNode *r = c->r; free(c); c = r; }
    }
    t->root = NULL; t->size = 0;
}
#endif
