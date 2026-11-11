#define _POSIX_C_SOURCE 200809L
/* q4_rb_bench.c - Task 4: red-black tree under the same 100000-key builds and
   the same scheduler workload that q2_avl_bench.c ran on the AVL tree. */
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "rbtree.h"

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
    printf("n = %d keys\n", n);
    printf("%-10s %-8s %-12s %-12s %-10s %-10s %-8s %-8s %-8s\n", "order", "levels", "rotations", "per insert",
           "recolours", "build ms", "case1", "case2", "case3");
    for (int pass = 0; pass < 2; pass++) {
        for (int i = 0; i < n; i++) keys[i] = i + 1;
        if (pass == 0)
            for (int i = n - 1; i > 0; i--) {
                int j = (int)(next_rand() % (unsigned long long)(i + 1));
                long long tmp = keys[i]; keys[i] = keys[j]; keys[j] = tmp;
            }
        RbTree t;
        rb_init(&t);
        double t0 = now_ms();
        for (int i = 0; i < n; i++) rb_insert(&t, keys[i]);
        double build = now_ms() - t0;
        printf("%-10s %-8d %-12ld %-12.3f %-10ld %-10.2f %-8ld %-8ld %-8ld valid=%s\n", pass == 0 ? "random" : "ascending",
               rb_levels(&t), rb_rotations(&t), (double)rb_rotations(&t) / n, t.recolor, build, t.case1, t.case2, t.case3,
               rb_check(&t) == 0 ? "yes" : "NO");
        rb_free(&t);
    }
    printf("reference: log2(n) = %.2f, proven bound 2*log2(n+1) = %.2f levels\n", log2(n), 2 * log2(n + 1.0));

    rng = 88172645463325252ULL;
    RbTree t;
    rb_init(&t);
    for (int id = 0; id < n; id++) {
        long long vr = (long long)(next_rand() % 1000000ULL);
        rb_insert(&t, (vr << 20) | id);
    }
    long ins_rot = 0, del_rot = 0, max_ins = 0, max_del = 0;
    double t0 = now_ms();
    for (int s = 0; s < steps; s++) {
        long long k = rb_minimum(&t, t.root)->key;
        long before = rb_rotations(&t);
        rb_delete(&t, k);
        long d = rb_rotations(&t) - before;
        del_rot += d; if (d > max_del) max_del = d;
        long long id = k & ((1 << 20) - 1), vr = (k >> 20) + 1 + (long long)(next_rand() % 1000ULL);
        before = rb_rotations(&t);
        rb_insert(&t, (vr << 20) | id);
        d = rb_rotations(&t) - before;
        ins_rot += d; if (d > max_ins) max_ins = d;
    }
    double el = now_ms() - t0;
    printf("\nScheduler workload (red-black): %d tasks, %d steps of delete-min + insert\n", n, steps);
    printf("rotations per delete = %.4f (max %ld), per insert = %.4f (max %ld), total time = %.1f ms, height = %d, valid = %s\n",
           (double)del_rot / steps, max_del, (double)ins_rot / steps, max_ins, el, rb_levels(&t), rb_check(&t) == 0 ? "yes" : "NO");
    rb_free(&t);
    free(keys);
    return 0;
}
