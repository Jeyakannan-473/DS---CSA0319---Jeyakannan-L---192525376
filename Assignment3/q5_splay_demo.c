/* q5_splay_demo.c - Task 5: splay rotation trace, boundary tests, a model
   test, and a numerical check of the access lemma (amortised analysis). */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "splay.h"

static void show(const char *label, const Splay *t)
{
    printf("%-26s ", label);
    sp_shape(t->root);
    printf("   [levels %d, valid %s]\n", sp_levels(t), sp_valid(t) ? "yes" : "NO");
}

/* subtree size of every node is needed for the potential function */
static double potential_node(const SNode *n, long *size)
{
    if (!n) { *size = 0; return 0; }
    long a, b;
    double pa = potential_node(n->l, &a), pb = potential_node(n->r, &b);
    *size = a + b + 1;
    return pa + pb + log2((double)*size);
}

static long subtree_size(const SNode *n) { return n ? 1 + subtree_size(n->l) + subtree_size(n->r) : 0; }

static SNode *find_node(SNode *n, int k)
{
    while (n && n->key != k) n = k < n->key ? n->l : n->r;
    return n;
}

int main(void)
{
    Splay t;
    sp_init(&t);
    printf("PART A - splay trace\n");
    for (int k = 1; k <= 7; k++) {
        sp_insert(&t, k);
        char buf[32];
        snprintf(buf, sizeof buf, "insert %d", k);
        show(buf, &t);
    }
    int accesses[] = {1, 4, 6, 4, 7};
    t.trace = 1;
    for (int i = 0; i < 5; i++) {
        long z = t.zig, zz = t.zigzig, za = t.zigzag;
        int path;
        printf("access %d:\n", accesses[i]);
        sp_access(&t, accesses[i], &path);
        char buf[48];
        snprintf(buf, sizeof buf, "  path = %d, root now %d", path, t.root->key);
        show(buf, &t);
        printf("  steps: zig %ld, zig-zig %ld, zig-zag %ld\n", t.zig - z, t.zigzig - zz, t.zigzag - za);
    }
    t.trace = 0;
    printf("totals: zig %ld, zig-zig %ld, zig-zag %ld, single rotations %ld\n", t.zig, t.zigzig, t.zigzag, t.rotations);
    sp_free(&t);

    printf("\nPART B - boundary tests\n");
    sp_init(&t);
    int path = -1;
    int f5 = sp_access(&t, 5, &path);
    int d5 = sp_delete(&t, 5);
    printf("empty tree: access(5) = %s, delete(5) = %d, levels = %d\n", f5 ? "found" : "not found", d5, sp_levels(&t));
    int a = sp_insert(&t, 10), b = sp_insert(&t, 10);
    printf("insert 10 -> %d, insert 10 again -> %d (duplicate, node splayed), size = %zu\n", a, b, t.size);
    for (int k = 20; k >= 5; k -= 5) sp_insert(&t, k);
    show("after inserting 20 15 10 5", &t);
    sp_access(&t, 12, &path);
    printf("access 12 (absent): last node on the path %d becomes the root\n", t.root->key);
    show("  tree", &t);
    int da = sp_delete(&t, 5);
    int db = sp_delete(&t, 20);
    int dc = sp_delete(&t, 99);
    printf("delete 5 (smallest) -> %d, delete 20 (largest) -> %d, delete 99 (absent) -> %d\n", da, db, dc);
    show("after the deletions", &t);
    sp_free(&t);

    printf("\nPART C - 100000 random operations against a model (keys 0..999)\n");
    sp_init(&t);
    static char present[1000];
    memset(present, 0, sizeof present);
    unsigned long long s = 88172645463325252ULL;
    long mism = 0, bad = 0;
    int count = 0;
    for (int step = 1; step <= 100000; step++) {
        s ^= s << 13; s ^= s >> 7; s ^= s << 17;
        int key = (int)(s % 1000), op = (int)((s >> 24) % 3);
        if (op == 0) {
            int r = sp_insert(&t, key);
            if (r != !present[key]) mism++;
            if (r) { present[key] = 1; count++; }
        } else if (op == 1) {
            int r = sp_delete(&t, key);
            if (r != present[key]) mism++;
            if (r) { present[key] = 0; count--; }
        } else if (sp_access(&t, key, NULL) != present[key]) mism++;
        if (step % 500 == 0 && (!sp_valid(&t) || (int)t.size != count)) bad++;
    }
    printf("final size = %zu (model %d), model mismatches = %ld, failed validations = %ld\n", t.size, count, mism, bad);
    sp_free(&t);

    printf("\nPART D - access lemma: amortised cost <= 3*(log2 n - log2 s(x)) + 1\n");
    sp_init(&t);
    int n = 1023;
    for (int i = 0; i < n; i++) {                     /* worst start: ascending insertion */
        sp_insert(&t, i);
    }
    long sz;
    double worst_slack = 1e9, sum_amort = 0;
    int viol = 0, N = 3000;
    long max_actual = 0;
    for (int i = 0; i < N; i++) {
        s ^= s << 13; s ^= s >> 7; s ^= s << 17;
        int key = (int)(s % (unsigned long long)n);
        SNode *x = find_node(t.root, key);
        double sx = (double)subtree_size(x);
        double phi0 = potential_node(t.root, &sz);
        long r0 = t.rotations;
        sp_access(&t, key, NULL);
        long actual = t.rotations - r0;
        double phi1 = potential_node(t.root, &sz);
        double amort = (double)actual + phi1 - phi0;
        double bound = 3 * (log2((double)n) - log2(sx)) + 1;
        if (amort > bound + 1e-9) viol++;
        if (bound - amort < worst_slack) worst_slack = bound - amort;
        sum_amort += amort;
        if (actual > max_actual) max_actual = actual;
    }
    printf("n = %d, %d random accesses starting from an ascending-built tree (a left-leaning path)\n", n, N);
    printf("accesses that violate the bound = %d, smallest slack = %.3f, mean amortised cost = %.2f, largest actual cost = %ld rotations\n",
           viol, worst_slack, sum_amort / N, max_actual);
    double start_phi = 0;
    for (int i = 1; i <= n; i++) start_phi += log2((double)i);
    printf("potential of the starting path = sum of log2 sizes = %.1f, bound per access 3*log2(n)+1 = %.2f\n",
           start_phi, 3 * log2((double)n) + 1);
    sp_free(&t);
    return 0;
}
