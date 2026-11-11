/* btree.h - Task 3: B-tree of int keys with minimum degree t (CLRS style).
   Every node holds between t-1 and 2t-1 keys (the root may hold fewer) and
   has one more child than keys. Order m = 2t is the maximum number of
   children, so t = 50 gives an order-100 tree (up to 99 keys per node).
   reads counts the nodes visited, i.e. the disk reads of a search. */
#ifndef BTREE_H
#define BTREE_H
#include <stdio.h>
#include <stdlib.h>

typedef struct BTNode {
    int n;                    /* number of keys stored */
    int leaf;
    int *key;                 /* 2t-1 slots */
    struct BTNode **child;    /* 2t slots */
} BTNode;

typedef struct {
    BTNode *root;
    int t;
    size_t keys, nodes;
    int levels;               /* number of levels, an empty tree has 0 */
    long splits;
    long reads;
} BTree;

static inline BTNode *bt_new_node(BTree *tr, int leaf)
{
    BTNode *x = malloc(sizeof *x);
    if (!x) exit(1);
    x->n = 0;
    x->leaf = leaf;
    x->key = malloc((size_t)(2 * tr->t - 1) * sizeof *x->key);
    x->child = malloc((size_t)(2 * tr->t) * sizeof *x->child);
    if (!x->key || !x->child) exit(1);
    tr->nodes++;
    return x;
}

static inline void bt_init(BTree *tr, int t)
{
    tr->t = t; tr->keys = 0; tr->nodes = 0; tr->levels = 0;
    tr->splits = 0; tr->reads = 0;
    tr->root = NULL;
}

/* returns 1 and the position when the key is in the tree; counts node reads */
static inline int bt_search(BTree *tr, int k)
{
    BTNode *x = tr->root;
    while (x) {
        tr->reads++;
        int i = 0;
        while (i < x->n && k > x->key[i]) i++;
        if (i < x->n && k == x->key[i]) return 1;
        if (x->leaf) return 0;
        x = x->child[i];
    }
    return 0;
}

/* split the full child y = x->child[i] around its median key */
static inline void bt_split_child(BTree *tr, BTNode *x, int i)
{
    int t = tr->t;
    BTNode *y = x->child[i];
    BTNode *z = bt_new_node(tr, y->leaf);
    z->n = t - 1;
    for (int j = 0; j < t - 1; j++) z->key[j] = y->key[j + t];          /* upper t-1 keys */
    if (!y->leaf)
        for (int j = 0; j < t; j++) z->child[j] = y->child[j + t];      /* upper t children */
    y->n = t - 1;
    for (int j = x->n; j > i; j--) x->child[j + 1] = x->child[j];
    x->child[i + 1] = z;
    for (int j = x->n - 1; j >= i; j--) x->key[j + 1] = x->key[j];
    x->key[i] = y->key[t - 1];                                          /* median moves up */
    x->n++;
    tr->splits++;
}

static inline void bt_insert_nonfull(BTree *tr, BTNode *x, int k)
{
    int i = x->n - 1;
    if (x->leaf) {
        while (i >= 0 && k < x->key[i]) { x->key[i + 1] = x->key[i]; i--; }
        x->key[i + 1] = k;
        x->n++;
        return;
    }
    while (i >= 0 && k < x->key[i]) i--;
    i++;
    if (x->child[i]->n == 2 * tr->t - 1) {
        bt_split_child(tr, x, i);
        if (k > x->key[i]) i++;
    }
    bt_insert_nonfull(tr, x->child[i], k);
}

/* returns 1 when inserted, 0 when the key is already present */
static inline int bt_insert(BTree *tr, int k)
{
    long saved = tr->reads;
    int dup = bt_search(tr, k);
    tr->reads = saved;
    if (dup) return 0;
    if (!tr->root) {
        tr->root = bt_new_node(tr, 1);
        tr->levels = 1;
    }
    if (tr->root->n == 2 * tr->t - 1) {                 /* root full: tree grows by one level */
        BTNode *s = bt_new_node(tr, 0);
        s->child[0] = tr->root;
        tr->root = s;
        bt_split_child(tr, s, 0);
        tr->levels++;
    }
    bt_insert_nonfull(tr, tr->root, k);
    tr->keys++;
    return 1;
}

/* checks order, key counts and equal leaf depth; returns leaf depth or -1 */
static inline int bt_check_node(const BTree *tr, const BTNode *x, int is_root, const int *lo, const int *hi)
{
    int t = tr->t;
    if (x->n > 2 * t - 1 || (!is_root && x->n < t - 1) || (is_root && x->n < 1)) return -1;
    for (int i = 0; i < x->n; i++) {
        if (i && x->key[i - 1] >= x->key[i]) return -1;
        if ((lo && x->key[i] <= *lo) || (hi && x->key[i] >= *hi)) return -1;
    }
    if (x->leaf) return 1;
    int depth = -2;
    for (int i = 0; i <= x->n; i++) {
        const int *l = i ? &x->key[i - 1] : lo;
        const int *h = i < x->n ? &x->key[i] : hi;
        int d = bt_check_node(tr, x->child[i], 0, l, h);
        if (d < 0) return -1;
        if (depth == -2) depth = d;
        else if (d != depth) return -1;
    }
    return depth + 1;
}

static inline int bt_valid(const BTree *tr)
{
    if (!tr->root) return 1;
    return bt_check_node(tr, tr->root, 1, NULL, NULL) == tr->levels;
}

static inline void bt_free_node(BTNode *x)
{
    if (!x) return;
    if (!x->leaf)
        for (int i = 0; i <= x->n; i++) bt_free_node(x->child[i]);
    free(x->key);
    free(x->child);
    free(x);
}

static inline void bt_free(BTree *tr) { bt_free_node(tr->root); tr->root = NULL; tr->nodes = 0; tr->keys = 0; }

/* prints the tree level by level: each node is shown as [k1 k2 ...] */
static inline void bt_print_levels(const BTree *tr)
{
    if (!tr->root) { printf("  (empty tree)\n"); return; }
    BTNode **cur = malloc(sizeof *cur), **nxt;
    size_t cn = 1, cap = 1;
    cur[0] = tr->root;
    int level = 1;
    while (cn) {
        printf("  L%d:", level);
        size_t nn = 0, ncap = cn * (size_t)(2 * tr->t);
        nxt = malloc(ncap * sizeof *nxt);
        for (size_t a = 0; a < cn; a++) {
            printf(" [");
            for (int i = 0; i < cur[a]->n; i++) printf("%s%d", i ? " " : "", cur[a]->key[i]);
            printf("]");
            if (!cur[a]->leaf)
                for (int i = 0; i <= cur[a]->n; i++) nxt[nn++] = cur[a]->child[i];
        }
        printf("\n");
        free(cur);
        cur = nxt; cn = nn; cap = ncap; level++;
    }
    (void)cap;
    free(cur);
}
#endif
