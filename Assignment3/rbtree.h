/* rbtree.h - Task 4: red-black tree of long long keys (no duplicates), CLRS
   algorithms with one shared black sentinel (nil). Counters record rotations,
   colour changes and which insertion fix-up case ran. */
#ifndef RBTREE_H
#define RBTREE_H
#include <stdio.h>
#include <stdlib.h>

typedef struct RNode {
    long long key;
    int red;                       /* 1 = red, 0 = black */
    struct RNode *l, *r, *p;
} RNode;

typedef struct {
    RNode *root, *nil, nilnode;
    size_t size;
    long cmps;
    long rot_l, rot_r, recolor;
    long case1, case2, case3;      /* insertion fix-up cases */
} RbTree;

static inline void rb_init(RbTree *t)
{
    t->nil = &t->nilnode;
    t->nilnode.red = 0;
    t->nilnode.l = t->nilnode.r = t->nilnode.p = t->nil;
    t->nilnode.key = 0;
    t->root = t->nil;
    t->size = 0; t->cmps = 0;
    t->rot_l = t->rot_r = t->recolor = 0;
    t->case1 = t->case2 = t->case3 = 0;
}

static inline long rb_rotations(const RbTree *t) { return t->rot_l + t->rot_r; }

static inline void rb_rotate_left(RbTree *t, RNode *x)
{
    RNode *y = x->r;
    x->r = y->l;
    if (y->l != t->nil) y->l->p = x;
    y->p = x->p;
    if (x->p == t->nil) t->root = y;
    else if (x == x->p->l) x->p->l = y;
    else x->p->r = y;
    y->l = x;
    x->p = y;
    t->rot_l++;
}

static inline void rb_rotate_right(RbTree *t, RNode *y)
{
    RNode *x = y->l;
    y->l = x->r;
    if (x->r != t->nil) x->r->p = y;
    x->p = y->p;
    if (y->p == t->nil) t->root = x;
    else if (y == y->p->l) y->p->l = x;
    else y->p->r = x;
    x->r = y;
    y->p = x;
    t->rot_r++;
}

static inline void rb_insert_fixup(RbTree *t, RNode *z)
{
    while (z->p->red) {
        RNode *g = z->p->p;
        if (z->p == g->l) {
            RNode *u = g->r;
            if (u->red) {                              /* case 1: red uncle, recolour */
                z->p->red = 0; u->red = 0; g->red = 1;
                t->recolor += 3; t->case1++;
                z = g;
            } else {
                if (z == z->p->r) {                    /* case 2: inner child, rotate to outer */
                    z = z->p;
                    rb_rotate_left(t, z);
                    t->case2++;
                }
                z->p->red = 0; g->red = 1;             /* case 3: outer child, rotate grandparent */
                t->recolor += 2; t->case3++;
                rb_rotate_right(t, g);
            }
        } else {
            RNode *u = g->l;
            if (u->red) {
                z->p->red = 0; u->red = 0; g->red = 1;
                t->recolor += 3; t->case1++;
                z = g;
            } else {
                if (z == z->p->l) {
                    z = z->p;
                    rb_rotate_right(t, z);
                    t->case2++;
                }
                z->p->red = 0; g->red = 1;
                t->recolor += 2; t->case3++;
                rb_rotate_left(t, g);
            }
        }
    }
    if (t->root->red) { t->root->red = 0; t->recolor++; }
}

/* returns 1 when inserted, 0 for a duplicate key */
static inline int rb_insert(RbTree *t, long long k)
{
    RNode *y = t->nil, *x = t->root;
    while (x != t->nil) {
        t->cmps++;
        y = x;
        if (k == x->key) return 0;
        x = k < x->key ? x->l : x->r;
    }
    RNode *z = malloc(sizeof *z);
    if (!z) exit(1);
    z->key = k; z->red = 1; z->l = z->r = t->nil; z->p = y;
    if (y == t->nil) t->root = z;
    else if (k < y->key) y->l = z;
    else y->r = z;
    t->size++;
    rb_insert_fixup(t, z);
    return 1;
}

static inline RNode *rb_search(RbTree *t, long long k)
{
    RNode *x = t->root;
    while (x != t->nil) {
        t->cmps++;
        if (k == x->key) return x;
        x = k < x->key ? x->l : x->r;
    }
    return NULL;
}

static inline RNode *rb_minimum(const RbTree *t, RNode *x)
{
    while (x->l != t->nil) x = x->l;
    return x;
}

static inline void rb_transplant(RbTree *t, RNode *u, RNode *v)
{
    if (u->p == t->nil) t->root = v;
    else if (u == u->p->l) u->p->l = v;
    else u->p->r = v;
    v->p = u->p;
}

static inline void rb_delete_fixup(RbTree *t, RNode *x)
{
    while (x != t->root && !x->red) {
        if (x == x->p->l) {
            RNode *w = x->p->r;
            if (w->red) {                                  /* case 1 */
                w->red = 0; x->p->red = 1;
                rb_rotate_left(t, x->p);
                w = x->p->r;
            }
            if (!w->l->red && !w->r->red) {                /* case 2 */
                w->red = 1;
                x = x->p;
            } else {
                if (!w->r->red) {                          /* case 3 */
                    w->l->red = 0; w->red = 1;
                    rb_rotate_right(t, w);
                    w = x->p->r;
                }
                w->red = x->p->red;                        /* case 4 */
                x->p->red = 0; w->r->red = 0;
                rb_rotate_left(t, x->p);
                x = t->root;
            }
        } else {
            RNode *w = x->p->l;
            if (w->red) {
                w->red = 0; x->p->red = 1;
                rb_rotate_right(t, x->p);
                w = x->p->l;
            }
            if (!w->r->red && !w->l->red) {
                w->red = 1;
                x = x->p;
            } else {
                if (!w->l->red) {
                    w->r->red = 0; w->red = 1;
                    rb_rotate_left(t, w);
                    w = x->p->l;
                }
                w->red = x->p->red;
                x->p->red = 0; w->l->red = 0;
                rb_rotate_right(t, x->p);
                x = t->root;
            }
        }
    }
    x->red = 0;
}

/* returns 1 when removed, 0 when the key was absent */
static inline int rb_delete(RbTree *t, long long k)
{
    RNode *z = rb_search(t, k);
    if (!z) return 0;
    RNode *y = z, *x;
    int y_was_red = y->red;
    if (z->l == t->nil) { x = z->r; rb_transplant(t, z, z->r); }
    else if (z->r == t->nil) { x = z->l; rb_transplant(t, z, z->l); }
    else {
        y = rb_minimum(t, z->r);
        y_was_red = y->red;
        x = y->r;
        if (y->p == z) x->p = y;
        else {
            rb_transplant(t, y, y->r);
            y->r = z->r; y->r->p = y;
        }
        rb_transplant(t, z, y);
        y->l = z->l; y->l->p = y;
        y->red = z->red;
    }
    free(z);
    t->size--;
    if (!y_was_red) rb_delete_fixup(t, x);
    t->nil->p = t->nil;
    return 1;
}

/* Property checker. Returns a bit mask of violated properties:
   1 = P2 root is red            2 = P3 the leaf sentinel is red
   4 = P4 a red node has a red child
   8 = P5 black heights differ   16 = BST order broken   32 = parent link wrong */
static inline int rb_check_node(const RbTree *t, const RNode *n, const long long *lo, const long long *hi,
                                int *mask, int *bh)
{
    if (n == t->nil) { *bh = 1; return 1; }
    if ((lo && n->key <= *lo) || (hi && n->key >= *hi)) *mask |= 16;
    if (n->l != t->nil && n->l->p != n) *mask |= 32;
    if (n->r != t->nil && n->r->p != n) *mask |= 32;
    if (n->red && (n->l->red || n->r->red)) *mask |= 4;
    int bl, br;
    rb_check_node(t, n->l, lo, &n->key, mask, &bl);
    rb_check_node(t, n->r, &n->key, hi, mask, &br);
    if (bl != br) *mask |= 8;
    *bh = bl + (n->red ? 0 : 1);
    return 1;
}

static inline int rb_check(const RbTree *t)
{
    int mask = 0, bh;
    if (t->root != t->nil && t->root->red) mask |= 1;
    if (t->nil->red) mask |= 2;
    rb_check_node(t, t->root, NULL, NULL, &mask, &bh);
    return mask;
}

static inline int rb_levels_node(const RbTree *t, const RNode *n)
{
    if (n == t->nil) return 0;
    int a = rb_levels_node(t, n->l), b = rb_levels_node(t, n->r);
    return 1 + (a > b ? a : b);
}

static inline int rb_levels(const RbTree *t) { return rb_levels_node(t, t->root); }

/* black height of the root: black nodes on a path to a leaf, not counting nil */
static inline int rb_black_height(const RbTree *t)
{
    int bh = 0;
    for (RNode *x = t->root; x != t->nil; x = x->l) if (!x->red) bh++;
    return bh;
}

static inline void rb_shape_node(const RbTree *t, const RNode *n)
{
    if (n == t->nil) { printf("-"); return; }
    printf("%lld:%c", n->key, n->red ? 'R' : 'B');
    if (n->l == t->nil && n->r == t->nil) return;
    printf("(");
    rb_shape_node(t, n->l); printf(","); rb_shape_node(t, n->r);
    printf(")");
}

static inline void rb_shape(const RbTree *t) { rb_shape_node(t, t->root); }

static inline void rb_free_node(const RbTree *t, RNode *n)
{
    if (n == t->nil) return;
    rb_free_node(t, n->l);
    rb_free_node(t, n->r);
    free(n);
}

static inline void rb_free(RbTree *t) { rb_free_node(t, t->root); t->root = t->nil; t->size = 0; }
#endif
