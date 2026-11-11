/* avl.h - Task 2: AVL tree of long long keys (no duplicates).
   Height is counted in nodes (empty = 0, leaf = 1). Balance factor
   bf = height(left) - height(right) must stay in {-1, 0, +1}.
   The four rotation cases are counted separately; single_rot counts the
   physical single rotations (LR and RL need two each). */
#ifndef AVL_H
#define AVL_H
#include <stdio.h>
#include <stdlib.h>

typedef struct ANode {
    long long key;
    int h;
    struct ANode *l, *r;
} ANode;

typedef struct {
    ANode *root;
    size_t size;
    long cmps;
    long ll, rr, lr, rl;
    long single_rot;
} Avl;

static inline void avl_init(Avl *t)
{
    t->root = NULL; t->size = 0; t->cmps = 0;
    t->ll = t->rr = t->lr = t->rl = 0; t->single_rot = 0;
}

static inline int avl_h(const ANode *n) { return n ? n->h : 0; }
static inline int avl_bf(const ANode *n) { return n ? avl_h(n->l) - avl_h(n->r) : 0; }

static inline void avl_update(ANode *n)
{
    int a = avl_h(n->l), b = avl_h(n->r);
    n->h = 1 + (a > b ? a : b);
}

/* right rotation about y: its left child x becomes the subtree root */
static inline ANode *avl_rot_right(Avl *t, ANode *y)
{
    ANode *x = y->l;
    y->l = x->r;                 /* x's right subtree moves to y's left */
    x->r = y;                    /* y becomes x's right child */
    avl_update(y);
    avl_update(x);
    t->single_rot++;
    return x;
}

/* left rotation about x: its right child y becomes the subtree root */
static inline ANode *avl_rot_left(Avl *t, ANode *x)
{
    ANode *y = x->r;
    x->r = y->l;                 /* y's left subtree moves to x's right */
    y->l = x;                    /* x becomes y's left child */
    avl_update(x);
    avl_update(y);
    t->single_rot++;
    return y;
}

static inline ANode *avl_rebalance(Avl *t, ANode *n)
{
    avl_update(n);
    int b = avl_bf(n);
    if (b > 1) {                                   /* left heavy */
        if (avl_bf(n->l) < 0) {                    /* LR: left child leans right */
            n->l = avl_rot_left(t, n->l);
            t->lr++;
        } else {
            t->ll++;                               /* LL */
        }
        return avl_rot_right(t, n);
    }
    if (b < -1) {                                  /* right heavy */
        if (avl_bf(n->r) > 0) {                    /* RL: right child leans left */
            n->r = avl_rot_right(t, n->r);
            t->rl++;
        } else {
            t->rr++;                               /* RR */
        }
        return avl_rot_left(t, n);
    }
    return n;
}

static inline ANode *avl_ins(Avl *t, ANode *n, long long k, int *added)
{
    if (!n) {
        ANode *x = malloc(sizeof *x);
        if (!x) exit(1);
        x->key = k; x->h = 1; x->l = x->r = NULL;
        *added = 1;
        return x;
    }
    t->cmps++;
    if (k < n->key) n->l = avl_ins(t, n->l, k, added);
    else if (k > n->key) n->r = avl_ins(t, n->r, k, added);
    else return n;                                 /* duplicate */
    return *added ? avl_rebalance(t, n) : n;
}

/* returns 1 when inserted, 0 for a duplicate key */
static inline int avl_insert(Avl *t, long long k)
{
    int added = 0;
    t->root = avl_ins(t, t->root, k, &added);
    t->size += (size_t)added;
    return added;
}

static inline ANode *avl_search(Avl *t, long long k)
{
    ANode *c = t->root;
    while (c) {
        t->cmps++;
        if (k == c->key) return c;
        c = k < c->key ? c->l : c->r;
    }
    return NULL;
}

static inline ANode *avl_min_node(ANode *n)
{
    while (n->l) n = n->l;
    return n;
}

static inline ANode *avl_del(Avl *t, ANode *n, long long k, int *removed)
{
    if (!n) return NULL;
    t->cmps++;
    if (k < n->key) n->l = avl_del(t, n->l, k, removed);
    else if (k > n->key) n->r = avl_del(t, n->r, k, removed);
    else {
        *removed = 1;
        if (!n->l || !n->r) {
            ANode *child = n->l ? n->l : n->r;
            free(n);
            return child;
        }
        ANode *s = avl_min_node(n->r);             /* inorder successor */
        n->key = s->key;
        int dummy = 0;
        n->r = avl_del(t, n->r, s->key, &dummy);
    }
    return avl_rebalance(t, n);
}

/* returns 1 when removed, 0 when the key was absent */
static inline int avl_delete(Avl *t, long long k)
{
    int removed = 0;
    t->root = avl_del(t, t->root, k, &removed);
    t->size -= (size_t)removed;
    return removed;
}

/* checks BST order, stored heights and |bf| <= 1; returns the height or -1 */
static inline int avl_check(const ANode *n, const long long *lo, const long long *hi)
{
    if (!n) return 0;
    if ((lo && n->key <= *lo) || (hi && n->key >= *hi)) return -1;
    int a = avl_check(n->l, lo, &n->key);
    if (a < 0) return -1;
    int b = avl_check(n->r, &n->key, hi);
    if (b < 0) return -1;
    int hh = 1 + (a > b ? a : b);
    if (hh != n->h || a - b > 1 || b - a > 1) return -1;
    return hh;
}

static inline int avl_valid(const Avl *t) { return avl_check(t->root, NULL, NULL) >= 0; }

static inline void avl_shape(const ANode *n)
{
    if (!n) { printf("-"); return; }
    if (!n->l && !n->r) { printf("%lld", n->key); return; }
    printf("%lld(", n->key);
    avl_shape(n->l); printf(","); avl_shape(n->r);
    printf(")");
}

static inline void avl_free_nodes(ANode *n)
{
    if (!n) return;
    avl_free_nodes(n->l);
    avl_free_nodes(n->r);
    free(n);
}

static inline void avl_free(Avl *t) { avl_free_nodes(t->root); t->root = NULL; t->size = 0; }
#endif
