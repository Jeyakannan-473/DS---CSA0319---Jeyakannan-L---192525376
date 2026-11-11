#define _POSIX_C_SOURCE 200809L
/* q2_avl_bench.c - Task 2: 100000-key comparison of a plain BST and an AVL
   tree (random and ascending insertion), plus a scheduler-style workload that
   is reused by Task 4. Counts are exact; times are one run on this machine. */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "avl.h"
#include "bst.h"

static unsigned long long rng = 88172645463325252ULL;
static unsigned long long next_rand(void)
{
    rng ^= rng << 13; rng ^= rng >> 7; rng ^= rng << 17;
    return rng;
}

static double now_ms(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec * 1e3 + (double)t.tv_nsec / 1e6;
}

static long avl_path_sum(const Avl *t)
{
    if (!t->root) return 0;
    ANode **q = malloc(t->size * sizeof *q);
    size_t head = 0, tail = 0;
    long sum = 0;
    int depth = 0;
    q[tail++] = t->root;
    while (head < tail) {
        size_t level = tail - head;
        depth++;
        while (level--) {
            ANode *x = q[head++];
            sum += depth;
            if (x->l) q[tail++] = x->l;
            if (x->r) q[tail++] = x->r;
        }
    }
    free(q);
    return sum;
}

int main(void)
{
#ifdef SMALL
    int n = 5000;
    int steps = 10000;
#else
    int n = 100000;
    int steps = 200000;
#endif
    long long *keys = malloc((size_t)n * sizeof *keys);
    int *sample = malloc(2000 * sizeof *sample);

    printf("n = %d keys; search time is the mean of 2000 searches for random stored keys\n", n);
    printf("%-9s %-10s %-8s %-12s %-14s %-12s %-12s\n", "tree", "order", "levels", "avg cmps",
           "build ms", "search ns", "build cmps");
    for (int pass = 0; pass < 2; pass++) {
        for (int i = 0; i < n; i++) keys[i] = i + 1;
        if (pass == 0)
            for (int i = n - 1; i > 0; i--) {
                int j = (int)(next_rand() % (unsigned long long)(i + 1));
                long long tmp = keys[i]; keys[i] = keys[j]; keys[j] = tmp;
            }
        for (int i = 0; i < 2000; i++) sample[i] = (int)keys[next_rand() % (unsigned long long)n];

        /* plain BST */
        Bst b;
        bst_init(&b);
        double t0 = now_ms();
        for (int i = 0; i < n; i++) bst_insert(&b, (int)keys[i]);
        double build = now_ms() - t0;
        long bc = b.cmps;
        double avg = (double)bst_path_sum(&b) / n;
        t0 = now_ms();
        for (int i = 0; i < 2000; i++) bst_search(&b, sample[i]);
        double sns = (now_ms() - t0) * 1e6 / 2000;
        printf("%-9s %-10s %-8d %-12.2f %-14.2f %-12.0f %-12ld\n", "BST", pass == 0 ? "random" : "ascending",
               bst_height(&b) + 1, avg, build, sns, bc);
        bst_free(&b);

        /* AVL tree */
        Avl a;
        avl_init(&a);
        t0 = now_ms();
        for (int i = 0; i < n; i++) avl_insert(&a, keys[i]);
        build = now_ms() - t0;
        bc = a.cmps;
        avg = (double)avl_path_sum(&a) / n;
        t0 = now_ms();
        for (int i = 0; i < 2000; i++) avl_search(&a, sample[i]);
        sns = (now_ms() - t0) * 1e6 / 2000;
        printf("%-9s %-10s %-8d %-12.2f %-14.2f %-12.0f %-12ld\n", "AVL", pass == 0 ? "random" : "ascending",
               avl_h(a.root), avg, build, sns, bc);
        printf("          rotations: LL = %ld, RR = %ld, LR = %ld, RL = %ld, single = %ld (%.3f per insert), valid = %s\n",
               a.ll, a.rr, a.lr, a.rl, a.single_rot, (double)a.single_rot / n, avl_valid(&a) ? "yes" : "NO");
        avl_free(&a);
    }
    printf("reference: log2(n) = %.2f, AVL height bound = %.2f levels\n", log2(n), 1.4405 * log2(n + 2) - 0.3277);

    /* scheduler workload: initial tasks, then repeatedly run the smallest key
       and re-insert it with a larger virtual runtime */
    rng = 88172645463325252ULL;
    Avl a;
    avl_init(&a);
    for (int id = 0; id < n; id++) {
        long long vr = (long long)(next_rand() % 1000000ULL);
        avl_insert(&a, (vr << 20) | id);
    }
    long ins_rot = 0, del_rot = 0, max_ins = 0, max_del = 0;
    double t0 = now_ms();
    for (int s = 0; s < steps; s++) {
        long long k = avl_min_node(a.root)->key;
        long before = a.single_rot;
        avl_delete(&a, k);
        long d = a.single_rot - before;
        del_rot += d; if (d > max_del) max_del = d;
        long long id = k & ((1 << 20) - 1), vr = (k >> 20) + 1 + (long long)(next_rand() % 1000ULL);
        before = a.single_rot;
        avl_insert(&a, (vr << 20) | id);
        d = a.single_rot - before;
        ins_rot += d; if (d > max_ins) max_ins = d;
    }
    double el = now_ms() - t0;
    printf("\nScheduler workload (AVL): %d tasks, %d steps of delete-min + insert\n", n, steps);
    printf("rotations per delete = %.4f (max %ld), per insert = %.4f (max %ld), total time = %.1f ms, height = %d, valid = %s\n",
           (double)del_rot / steps, max_del, (double)ins_rot / steps, max_ins, el, avl_h(a.root), avl_valid(&a) ? "yes" : "NO");
    avl_free(&a);
    free(keys); free(sample);
    return 0;
}
