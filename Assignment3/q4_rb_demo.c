/* q4_rb_demo.c - Task 4: red-black insertion trace, property checker tests,
   boundary tests and a randomised test against a model. */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "rbtree.h"

static void show(const RbTree *t)
{
    printf("   ");
    rb_shape(t);
    printf("   [levels %d, valid %s]\n", rb_levels(t), rb_check(t) == 0 ? "yes" : "NO");
}

static const char *describe(int mask, char *buf)
{
    buf[0] = '\0';
    if (!mask) return "no violation";
    if (mask & 1) strcat(buf, "P2(root red) ");
    if (mask & 2) strcat(buf, "P3(leaf red) ");
    if (mask & 4) strcat(buf, "P4(red-red) ");
    if (mask & 8) strcat(buf, "P5(black height) ");
    if (mask & 16) strcat(buf, "BST-order ");
    if (mask & 32) strcat(buf, "parent-link ");
    return buf;
}

int main(void)
{
    RbTree t;
    rb_init(&t);
    long long keys[] = {7, 3, 18, 10, 22, 8, 11, 26, 2, 6, 13};
    printf("PART A - insertion trace (R = red, B = black)\n");
    for (int i = 0; i < 11; i++) {
        long c1 = t.case1, c2 = t.case2, c3 = t.case3, rl = t.rot_l, rr = t.rot_r, rc = t.recolor;
        rb_insert(&t, keys[i]);
        printf("insert %-3lld case1 = %ld, case2 = %ld, case3 = %ld, rotations = %ld, recolours = %ld\n",
               keys[i], t.case1 - c1, t.case2 - c2, t.case3 - c3, (t.rot_l - rl) + (t.rot_r - rr), t.recolor - rc);
        show(&t);
    }
    printf("totals: rotations = %ld (left %ld, right %ld), recolours = %ld, black height of root = %d\n",
           rb_rotations(&t), t.rot_l, t.rot_r, t.recolor, rb_black_height(&t));

    printf("\nPART B - the property checker detects deliberate damage\n");
    char buf[96];
    printf("intact tree             : %s\n", describe(rb_check(&t), buf));
    t.root->red = 1;
    printf("root painted red        : %s\n", describe(rb_check(&t), buf));
    t.root->red = 0;
    t.nilnode.red = 1;
    printf("leaf sentinel red       : %s\n", describe(rb_check(&t), buf));
    t.nilnode.red = 0;
    RNode *n11 = rb_search(&t, 11);
    n11->red = 1;                                   /* 11 becomes red above its red child 13 */
    printf("11 painted red          : %s\n", describe(rb_check(&t), buf));
    n11->red = 0;
    RNode *n3 = rb_search(&t, 3);
    n3->red = 1;
    printf("3 painted red           : %s\n", describe(rb_check(&t), buf));
    n3->red = 0;
    RNode *n8 = rb_search(&t, 8);
    n8->key = 100;
    printf("key 8 changed to 100    : %s\n", describe(rb_check(&t), buf));
    n8->key = 8;
    printf("after restoring all     : %s\n", describe(rb_check(&t), buf));
    rb_free(&t);

    printf("\nPART C - ascending insertion 1..15\n");
    rb_init(&t);
    for (long long k = 1; k <= 15; k++) rb_insert(&t, k);
    show(&t);
    printf("rotations = %ld, recolours = %ld\n", rb_rotations(&t), t.recolor);
    rb_free(&t);

    printf("\nPART D - boundary tests\n");
    rb_init(&t);
    printf("empty tree: levels = %d, search(5) = %s, delete(5) = %d, valid = %s\n", rb_levels(&t),
           rb_search(&t, 5) ? "found" : "not found", rb_delete(&t, 5), rb_check(&t) == 0 ? "yes" : "NO");
    int a = rb_insert(&t, 5);
    int b = rb_insert(&t, 5);
    printf("insert 5 -> %d, insert 5 again -> %d (duplicate), size = %zu, root colour = %s\n", a, b, t.size,
           t.root->red ? "red" : "black");
    rb_delete(&t, 5);
    printf("delete the only node: size = %zu, levels = %d, valid = %s\n", t.size, rb_levels(&t), rb_check(&t) == 0 ? "yes" : "NO");
    rb_free(&t);

    printf("\nPART E - 200000 random insert/delete/search operations against a model\n");
    rb_init(&t);
    static char present[3000];
    memset(present, 0, sizeof present);
    unsigned long long s = 88172645463325252ULL;
    long mism = 0, bad = 0;
    int count = 0;
    for (int step = 1; step <= 200000; step++) {
        s ^= s << 13; s ^= s >> 7; s ^= s << 17;
        int key = (int)(s % 3000), op = (int)((s >> 24) % 3);
        if (op == 0) {
            int r = rb_insert(&t, key);
            if (r != !present[key]) mism++;
            if (r) { present[key] = 1; count++; }
        } else if (op == 1) {
            int r = rb_delete(&t, key);
            if (r != present[key]) mism++;
            if (r) { present[key] = 0; count--; }
        } else if ((rb_search(&t, key) != NULL) != present[key]) mism++;
        if (step % 500 == 0 && (rb_check(&t) != 0 || (int)t.size != count)) bad++;
    }
    printf("final size = %zu (model %d), levels = %d, model mismatches = %ld, failed property checks = %ld\n",
           t.size, count, rb_levels(&t), mism, bad);
    rb_free(&t);

    printf("\nPART F - height against the proven bound 2*log2(n+1)\n");
    long ns[] = {1000, 100000};
    for (int ni = 0; ni < 2; ni++)
        for (int pass = 0; pass < 2; pass++) {
            rb_init(&t);
            long n2 = ns[ni];
            for (long i = 1; i <= n2; i++) {
                long long k = i;
                if (pass == 0) { s ^= s << 13; s ^= s >> 7; s ^= s << 17; k = (long long)(s % 1000000007ULL); }
                rb_insert(&t, k);
            }
            printf("n = %-7ld %-10s size = %-7zu levels = %-3d bound = %.1f black height = %d valid = %s\n", n2,
                   pass == 0 ? "random" : "ascending", t.size, rb_levels(&t), 2 * log2((double)t.size + 1),
                   rb_black_height(&t), rb_check(&t) == 0 ? "yes" : "NO");
            rb_free(&t);
        }
    return 0;
}
