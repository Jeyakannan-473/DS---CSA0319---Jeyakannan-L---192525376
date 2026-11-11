/* bst.h - Task 1: binary search tree of int keys (no duplicates).
   Insert, search and delete are iterative, so a degenerate tree of
   100,000 nodes cannot overflow the call stack. cmps counts key comparisons. */
#ifndef BST_H
#define BST_H
#include <stdio.h>
#include <stdlib.h>

typedef struct BNode {
    int key;
    struct BNode *l, *r;
} BNode;

typedef struct {
    BNode *root;
    size_t size;
    long cmps;
} Bst;

static inline void bst_init(Bst *t) { t->root = NULL; t->size = 0; t->cmps = 0; }

/* returns 1 when inserted, 0 when the key already exists */
static inline int bst_insert(Bst *t, int k)
{
    BNode *p = NULL, *c = t->root;
    while (c) {
        t->cmps++;
        if (k == c->key) return 0;
        p = c;
        c = k < c->key ? c->l : c->r;
    }
    BNode *n = malloc(sizeof *n);
    if (!n) exit(1);
    n->key = k; n->l = n->r = NULL;
    if (!p) t->root = n;
    else if (k < p->key) p->l = n;
    else p->r = n;
    t->size++;
    return 1;
}

static inline BNode *bst_search(Bst *t, int k)
{
    BNode *c = t->root;
    while (c) {
        t->cmps++;
        if (k == c->key) return c;
        c = k < c->key ? c->l : c->r;
    }
    return NULL;
}

/* returns 0 = key absent, 1 = leaf removed, 2 = node with one child removed,
   3 = node with two children (inorder successor copied up, then removed) */
static inline int bst_delete(Bst *t, int k)
{
    BNode *p = NULL, *c = t->root;
    while (c && c->key != k) {
        t->cmps++;
        p = c;
        c = k < c->key ? c->l : c->r;
    }
    if (!c) return 0;
    t->cmps++;
    int kind;
    if (c->l && c->r) {
        BNode *sp = c, *s = c->r;
        while (s->l) { sp = s; s = s->l; }
        c->key = s->key;
        if (sp == c) sp->r = s->r; else sp->l = s->r;
        free(s);
        kind = 3;
    } else {
        BNode *child = c->l ? c->l : c->r;
        kind = child ? 2 : 1;
        if (!p) t->root = child;
        else if (p->l == c) p->l = child;
        else p->r = child;
        free(c);
    }
    t->size--;
    return kind;
}

static inline BNode *bst_min(const Bst *t)
{
    BNode *c = t->root;
    while (c && c->l) c = c->l;
    return c;
}

static inline BNode *bst_max(const Bst *t)
{
    BNode *c = t->root;
    while (c && c->r) c = c->r;
    return c;
}

/* height in edges; an empty tree has height -1. Level-order, no recursion. */
static inline int bst_height(const Bst *t)
{
    if (!t->root) return -1;
    BNode **q = malloc(t->size * sizeof *q);
    size_t head = 0, tail = 0;
    int h = -1;
    q[tail++] = t->root;
    while (head < tail) {
        size_t level = tail - head;
        h++;
        while (level--) {
            BNode *x = q[head++];
            if (x->l) q[tail++] = x->l;
            if (x->r) q[tail++] = x->r;
        }
    }
    free(q);
    return h;
}

/* sum over all nodes of (depth + 1): comparisons for searching every key once */
static inline long bst_path_sum(const Bst *t)
{
    if (!t->root) return 0;
    BNode **q = malloc(t->size * sizeof *q);
    size_t head = 0, tail = 0;
    long sum = 0;
    int depth = 0;
    q[tail++] = t->root;
    while (head < tail) {
        size_t level = tail - head;
        depth++;
        while (level--) {
            BNode *x = q[head++];
            sum += depth;
            if (x->l) q[tail++] = x->l;
            if (x->r) q[tail++] = x->r;
        }
    }
    free(q);
    return sum;
}

typedef void (*bst_visit)(int key, void *ctx);

static inline void bst_inorder(const BNode *n, bst_visit f, void *ctx)
{
    if (!n) return;
    bst_inorder(n->l, f, ctx); f(n->key, ctx); bst_inorder(n->r, f, ctx);
}

static inline void bst_preorder(const BNode *n, bst_visit f, void *ctx)
{
    if (!n) return;
    f(n->key, ctx); bst_preorder(n->l, f, ctx); bst_preorder(n->r, f, ctx);
}

static inline void bst_postorder(const BNode *n, bst_visit f, void *ctx)
{
    if (!n) return;
    bst_postorder(n->l, f, ctx); bst_postorder(n->r, f, ctx); f(n->key, ctx);
}

static inline void bst_levelorder(const Bst *t, bst_visit f, void *ctx)
{
    if (!t->root) return;
    BNode **q = malloc(t->size * sizeof *q);
    size_t head = 0, tail = 0;
    q[tail++] = t->root;
    while (head < tail) {
        BNode *x = q[head++];
        f(x->key, ctx);
        if (x->l) q[tail++] = x->l;
        if (x->r) q[tail++] = x->r;
    }
    free(q);
}

/* nested form used for drawings: key(left,right), '-' marks an empty child */
static inline void bst_shape(const BNode *n)
{
    if (!n) { printf("-"); return; }
    if (!n->l && !n->r) { printf("%d", n->key); return; }
    printf("%d(", n->key);
    bst_shape(n->l); printf(","); bst_shape(n->r);
    printf(")");
}

/* frees every node without recursion by rotating the left child up */
static inline void bst_free(Bst *t)
{
    BNode *c = t->root;
    while (c) {
        if (c->l) {
            BNode *l = c->l;
            c->l = l->r;
            l->r = c;
            c = l;
        } else {
            BNode *r = c->r;
            free(c);
            c = r;
        }
    }
    t->root = NULL;
    t->size = 0;
}

/* 1 when the tree is a valid BST: inorder keys strictly increasing */
static inline int bst_valid(const Bst *t)
{
    if (!t->root) return 1;
    BNode **stack = malloc((t->size + 1) * sizeof *stack);
    size_t top = 0, count = 0;
    BNode *c = t->root;
    long prev = -2147483649L;
    int ok = 1;
    while (c || top) {
        while (c) { stack[top++] = c; c = c->l; }
        c = stack[--top];
        if ((long)c->key <= prev) ok = 0;
        prev = c->key;
        count++;
        c = c->r;
    }
    free(stack);
    return ok && count == t->size;
}
#endif
