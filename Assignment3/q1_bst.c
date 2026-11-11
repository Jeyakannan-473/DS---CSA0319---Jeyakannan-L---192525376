/* q1_bst.c - Task 1: BST demonstration and test suite.
   Part A: traversals, search and delete cases on a 10-key tree.
   Part B: boundary tests (empty tree, duplicates, extreme keys, missing keys).
   Part C: 100000 random operations checked against a simple model. */
#include <stdio.h>
#include <string.h>
#include "bst.h"

static void print_key(int k, void *ctx)
{
    (void)ctx;
    printf(" %d", k);
}

static void shape(const char *label, const Bst *t)
{
    printf("%-28s ", label);
    bst_shape(t->root);
    printf("   [size %zu, height %d]\n", t->size, bst_height(t));
}

int main(void)
{
    Bst t;
    bst_init(&t);
    int keys[] = {50, 30, 70, 20, 40, 60, 80, 35, 45, 65};
    printf("PART A - operations on a 10-key tree\n");
    for (int i = 0; i < 10; i++) {
        long before = t.cmps;
        bst_insert(&t, keys[i]);
        printf("insert %-3d comparisons = %ld\n", keys[i], t.cmps - before);
    }
    shape("initial tree", &t);
    printf("inorder   :"); bst_inorder(t.root, print_key, NULL); printf("\n");
    printf("preorder  :"); bst_preorder(t.root, print_key, NULL); printf("\n");
    printf("postorder :"); bst_postorder(t.root, print_key, NULL); printf("\n");
    printf("levelorder:"); bst_levelorder(&t, print_key, NULL); printf("\n");
    printf("min = %d, max = %d\n", bst_min(&t)->key, bst_max(&t)->key);

    int probe[] = {45, 65, 99, 10};
    for (int i = 0; i < 4; i++) {
        long before = t.cmps;
        BNode *f = bst_search(&t, probe[i]);
        printf("search %-3d -> %-9s comparisons = %ld\n", probe[i], f ? "found" : "not found", t.cmps - before);
    }

    printf("\nDeletion cases\n");
    const char *name[] = {"absent", "leaf", "one child", "two children"};
    int del[] = {20, 30, 50};
    for (int i = 0; i < 3; i++) {
        int kind = bst_delete(&t, del[i]);
        printf("delete %-3d -> case %d (%s)\n", del[i], kind, name[kind]);
        shape("  tree now", &t);
    }
    int kind = bst_delete(&t, 99);
    printf("delete 99  -> case %d (%s)\n", kind, name[kind]);
    printf("valid BST after deletions: %s\n", bst_valid(&t) ? "yes" : "NO");
    bst_free(&t);

    printf("\nPART B - boundary tests\n");
    bst_init(&t);
    int d5 = bst_delete(&t, 5);
    printf("empty tree: height = %d, size = %zu, search(5) = %s, delete(5) = %d, min = %s\n",
           bst_height(&t), t.size, bst_search(&t, 5) ? "found" : "not found", d5,
           bst_min(&t) ? "exists" : "none");
    printf("empty tree traversal output: \""); bst_inorder(t.root, print_key, NULL); printf("\"\n");
    int r1 = bst_insert(&t, 10);
    int r2 = bst_insert(&t, 10);
    printf("insert 10 -> %d, insert 10 again -> %d (duplicate rejected), size = %zu\n", r1, r2, t.size);
    int r3 = bst_delete(&t, 10);
    printf("delete 10 (only node) -> case %d, tree is %s, height = %d\n", r3,
           t.root ? "not empty" : "empty", bst_height(&t));
    bst_insert(&t, 0); bst_insert(&t, 2147483647); bst_insert(&t, -2147483647 - 1);
    printf("extreme keys: min = %d, max = %d, size = %zu\n", bst_min(&t)->key, bst_max(&t)->key, t.size);
    printf("search INT_MIN = %s, search INT_MAX = %s, search -1 = %s\n",
           bst_search(&t, -2147483647 - 1) ? "found" : "not found",
           bst_search(&t, 2147483647) ? "found" : "not found",
           bst_search(&t, -1) ? "found" : "not found");
    bst_free(&t);

    printf("\nPART C - 100000 random operations against a model (keys 0..999)\n");
    bst_init(&t);
    static char present[1000];
    memset(present, 0, sizeof present);
    unsigned long long s = 88172645463325252ULL;
    long mismatches = 0, ins = 0, del_ok = 0, hits = 0;
    int count = 0;
    for (int step = 1; step <= 100000; step++) {
        s ^= s << 13; s ^= s >> 7; s ^= s << 17;
        int key = (int)(s % 1000), op = (int)((s >> 20) % 3);
        if (op == 0) {
            int r = bst_insert(&t, key);
            if (r != !present[key]) mismatches++;
            if (r) { present[key] = 1; count++; ins++; }
        } else if (op == 1) {
            int r = bst_delete(&t, key);
            if ((r != 0) != present[key]) mismatches++;
            if (r) { present[key] = 0; count--; del_ok++; }
        } else {
            int r = bst_search(&t, key) != NULL;
            if (r != present[key]) mismatches++;
            hits += r;
        }
        if (step % 1000 == 0 && (!bst_valid(&t) || (int)t.size != count)) mismatches++;
    }
    printf("insertions accepted = %ld, deletions done = %ld, successful searches = %ld\n", ins, del_ok, hits);
    printf("final size = %zu (model %d), height = %d, mismatches with the model = %ld\n",
           t.size, count, bst_height(&t), mismatches);
    bst_free(&t);
    return 0;
}
