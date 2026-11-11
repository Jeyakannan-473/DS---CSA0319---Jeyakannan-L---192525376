/* q5_workload.c - Task 5: 10000-access workloads on a splay tree of 1000 keys,
   compared with a perfectly balanced static BST and a move-to-front list
   (the data structure behind an LRU ordering). Every number is measured. */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "splay.h"

#define N 1000
#define M 10000

static unsigned long long rng = 88172645463325252ULL;
static unsigned long long next_rand(void)
{
    rng ^= rng << 13; rng ^= rng >> 7; rng ^= rng << 17;
    return rng;
}

/* depth (nodes visited) of key k in a perfectly balanced BST over 1..N */
static int balanced_visits(int k)
{
    int lo = 1, hi = N, v = 0;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        v++;
        if (mid == k) return v;
        if (k < mid) hi = mid - 1; else lo = mid + 1;
    }
    return v;
}

int main(void)
{
    const char *names[] = {"uniform random", "skewed 90/10", "sequential sweep", "single hot key"};
    static int seq[4][M];
    int hot[100];
    int perm[N];
    for (int i = 0; i < N; i++) perm[i] = i + 1;
    for (int i = N - 1; i > 0; i--) {
        int j = (int)(next_rand() % (unsigned long long)(i + 1));
        int tmp = perm[i]; perm[i] = perm[j]; perm[j] = tmp;
    }
    for (int i = 0; i < 100; i++) hot[i] = perm[i];
    for (int i = 0; i < M; i++) {
        seq[0][i] = (int)(next_rand() % N) + 1;
        seq[1][i] = (next_rand() % 100 < 90) ? hot[next_rand() % 100] : (int)(next_rand() % N) + 1;
        seq[2][i] = i % N + 1;
        seq[3][i] = 500;
    }

    printf("n = %d keys inserted in ascending order (a left-leaning path), then %d accesses per workload\n", N, M);
    printf("%-18s %-9s %-9s %-9s %-9s %-10s %-10s %-9s %-9s %-9s\n", "workload", "avg path", "last 5000", "worst",
           "rotations", "static BST", "MTF list", "LRU hit", "path<=8", "levels");
    static double blocks[4][20];
    for (int w = 0; w < 4; w++) {
        Splay t;
        sp_init(&t);
        for (int k = 1; k <= N; k++) sp_insert(&t, k);
        long r0 = t.rotations;
        long sum = 0, sum_last = 0, bal = 0, mtf_sum = 0, within8 = 0, hits100 = 0;
        int worst = 0, worst_at = 0, mtf[N];
        for (int i = 0; i < N; i++) mtf[i] = i + 1;
        for (int i = 0; i < M; i++) {
            int path;
            sp_access(&t, seq[w][i], &path);
            sum += path;
            if (i >= M / 2) sum_last += path;
            if (path > worst) { worst = path; worst_at = i + 1; }
            if (path <= 8) within8++;
            blocks[w][i / 500] += path / 500.0;
            bal += balanced_visits(seq[w][i]);
            int pos = 0;                                  /* move-to-front scan */
            while (mtf[pos] != seq[w][i]) pos++;
            mtf_sum += pos + 1;
            if (pos + 1 <= 100) hits100++;
            memmove(mtf + 1, mtf, (size_t)pos * sizeof mtf[0]);
            mtf[0] = seq[w][i];
        }
        printf("%-18s %-9.2f %-9.2f %-9d %-9ld %-10.2f %-10.2f %-9.3f %-9.3f %-9d  valid=%s worst@%d\n", names[w],
               (double)sum / M, (double)sum_last / (M / 2), worst, t.rotations - r0, (double)bal / M,
               (double)mtf_sum / M, (double)hits100 / M, (double)within8 / M, sp_levels(&t), sp_valid(&t) ? "yes" : "NO", worst_at);
        sp_free(&t);
    }

    printf("\nAverage path length per block of 500 accesses\n");
    printf("%-8s %-16s %-16s %-18s %-16s\n", "block", names[0], names[1], names[2], names[3]);
    for (int b = 0; b < 20; b++)
        printf("%-8d %-16.2f %-16.2f %-18.2f %-16.2f\n", b + 1, blocks[0][b], blocks[1][b], blocks[2][b], blocks[3][b]);

    printf("\nRoot history for the first 12 accesses of the skewed workload\n");
    Splay t;
    sp_init(&t);
    for (int k = 1; k <= N; k++) sp_insert(&t, k);
    printf("%-8s %-10s %-10s %-10s\n", "access", "key", "path", "root after");
    for (int i = 0; i < 12; i++) {
        int path;
        sp_access(&t, seq[1][i], &path);
        printf("%-8d %-10d %-10d %-10d\n", i + 1, seq[1][i], path, t.root->key);
    }
    sp_free(&t);
    return 0;
}
