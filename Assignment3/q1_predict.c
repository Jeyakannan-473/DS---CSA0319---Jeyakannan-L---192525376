/* q1_predict.c - Task 1: predict-the-output program for a binary search tree */
#include <stdio.h>
#include "bst.h"

static void print_key(int k, void *ctx)
{
    (void)ctx;
    printf(" %d", k);
}

static void show(const char *label, const Bst *t)
{
    printf("%s\n", label);
    printf("  inorder   :"); bst_inorder(t->root, print_key, NULL); printf("\n");
    printf("  preorder  :"); bst_preorder(t->root, print_key, NULL); printf("\n");
    printf("  postorder :"); bst_postorder(t->root, print_key, NULL); printf("\n");
    printf("  levelorder:"); bst_levelorder(t, print_key, NULL); printf("\n");
    printf("  height = %d, size = %zu\n", bst_height(t), t->size);
}

int main(void)
{
    int keys[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
    Bst t;
    bst_init(&t);
    for (int i = 0; i < 9; i++) bst_insert(&t, keys[i]);
    show("Tree after inserting 8 3 10 1 6 14 4 7 13", &t);

    printf("delete 3  -> case %d\n", bst_delete(&t, 3));
    printf("delete 8  -> case %d\n", bst_delete(&t, 8));
    printf("delete 99 -> case %d\n", bst_delete(&t, 99));
    show("Tree after the deletions", &t);
    bst_free(&t);
    return 0;
}
