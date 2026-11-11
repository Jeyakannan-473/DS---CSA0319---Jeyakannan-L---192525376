/* q2_avl_demo.c - Task 2: the four AVL rotations, trace, validation and
   boundary tests. Every tree is checked with avl_valid(). */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "avl.h"

static void show(const char *label, const Avl *t)
{
    printf("%-34s ", label);
    avl_shape(t->root);
    printf("   [height %d, valid %s]\n", avl_h(t->root), avl_valid(t) ? "yes" : "NO");
}

static void counters(const Avl *t)
{
    printf("   rotations: LL = %ld, RR = %ld, LR = %ld, RL = %ld, single rotations = %ld\n",
           t->ll, t->rr, t->lr, t->rl, t->single_rot);
}

static void case_demo(const char *name, const long long *keys, int n)
{
    Avl t;
    avl_init(&t);
    printf("%s: insert", name);
    for (int i = 0; i < n; i++) printf(" %lld", keys[i]);
    printf("\n");
    for (int i = 0; i < n; i++) {
        avl_insert(&t, keys[i]);
        char buf[48];
        snprintf(buf, sizeof buf, "  after inserting %lld", keys[i]);
        show(buf, &t);
    }
    counters(&t);
    avl_free(&t);
}

int main(void)
{
    printf("PART A - the four rotations with three keys\n");
    long long ll[] = {30, 20, 10}, rr[] = {10, 20, 30}, lr[] = {30, 10, 20}, rl[] = {10, 30, 20};
    case_demo("LL case (single right rotation)", ll, 3);
    case_demo("RR case (single left rotation)", rr, 3);
    case_demo("LR case (left then right rotation)", lr, 3);
    case_demo("RL case (right then left rotation)", rl, 3);

    printf("\nPART A2 - rotations with subtrees attached\n");
    long long ll2[] = {50, 30, 70, 20, 40, 10};
    long long lr2[] = {50, 30, 70, 20, 40, 35};
    case_demo("LL with subtrees", ll2, 6);
    case_demo("LR with subtrees", lr2, 6);

    printf("\nPART B - ascending insertion 1..9 (worst case for a plain BST)\n");
    Avl t;
    avl_init(&t);
    for (long long k = 1; k <= 9; k++) {
        long before = t.single_rot;
        avl_insert(&t, k);
        char buf[48];
        snprintf(buf, sizeof buf, "  insert %lld (+%ld rotations)", k, t.single_rot - before);
        show(buf, &t);
    }
    counters(&t);
    avl_free(&t);

    printf("\nPART C - validation on larger trees\n");
    int sizes[] = {1000, 100000};
    unsigned long long s = 88172645463325252ULL;
    for (int si = 0; si < 2; si++) {
        int n = sizes[si];
        for (int pass = 0; pass < 2; pass++) {
            avl_init(&t);
            for (int i = 1; i <= n; i++) {
                long long k = i;
                if (pass == 0) { s ^= s << 13; s ^= s >> 7; s ^= s << 17; k = (long long)(s % 1000000007ULL); }
                avl_insert(&t, k);
            }
            double bound = 1.4405 * log2((double)t.size + 2) - 0.3277;
            printf("n = %-7d %-10s size = %-7zu height = %-3d (upper bound %.1f, log2 n = %.1f) valid = %s\n",
                   n, pass == 0 ? "random" : "ascending", t.size, avl_h(t.root), bound, log2((double)t.size),
                   avl_valid(&t) ? "yes" : "NO");
            avl_free(&t);
        }
    }

    printf("\nPART D - deletion and boundary tests\n");
    avl_init(&t);
    printf("empty tree: height = %d, search(1) = %s, delete(1) = %d, valid = %s\n", avl_h(t.root),
           avl_search(&t, 1) ? "found" : "not found", avl_delete(&t, 1), avl_valid(&t) ? "yes" : "NO");
    int a = avl_insert(&t, 7);
    int b = avl_insert(&t, 7);
    printf("insert 7 -> %d, insert 7 again -> %d (duplicate), size = %zu\n", a, b, t.size);
    avl_delete(&t, 7);
    printf("after deleting the only key: size = %zu, height = %d\n", t.size, avl_h(t.root));
    for (long long k = 1; k <= 100; k++) avl_insert(&t, k);
    for (long long k = 2; k <= 100; k += 2) avl_delete(&t, k);
    printf("1..100 inserted, evens deleted: size = %zu, height = %d, valid = %s\n", t.size, avl_h(t.root),
           avl_valid(&t) ? "yes" : "NO");
    int missing = avl_delete(&t, 1000);
    printf("delete missing key 1000 -> %d, smallest key 1 %s, largest key 99 %s\n", missing,
           avl_search(&t, 1) ? "found" : "not found", avl_search(&t, 99) ? "found" : "not found");
    avl_free(&t);

    printf("\nPART E - 200000 random insert/delete/search operations against a model\n");
    avl_init(&t);
    static char present[2000];
    memset(present, 0, sizeof present);
    long mism = 0, bad_tree = 0;
    int count = 0;
    for (int step = 1; step <= 200000; step++) {
        s ^= s << 13; s ^= s >> 7; s ^= s << 17;
        int key = (int)(s % 2000), op = (int)((s >> 24) % 3);
        if (op == 0) {
            int r = avl_insert(&t, key);
            if (r != !present[key]) mism++;
            if (r) { present[key] = 1; count++; }
        } else if (op == 1) {
            int r = avl_delete(&t, key);
            if (r != present[key]) mism++;
            if (r) { present[key] = 0; count--; }
        } else if ((avl_search(&t, key) != NULL) != present[key]) mism++;
        if (step % 1000 == 0 && (!avl_valid(&t) || (int)t.size != count)) bad_tree++;
    }
    printf("final size = %zu (model %d), height = %d, model mismatches = %ld, failed validations = %ld\n",
           t.size, count, avl_h(t.root), mism, bad_tree);
    counters(&t);
    avl_free(&t);
    return 0;
}
