#define _POSIX_C_SOURCE 200809L
/* q1_experiment.c - Task 1: random versus sorted insertion, and the
   alternative of a sorted array. All counts are measured. */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
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

int main(void)
{
    int sizes[] = {1000, 10000, 30000};
    printf("%-8s %-12s %-8s %-14s %-14s %-12s %-12s\n", "n", "insert order", "height",
           "build cmps", "avg search cmp", "theory avg", "build ms");
    for (int si = 0; si < 3; si++) {
        int n = sizes[si];
        int *keys = malloc((size_t)n * sizeof *keys);
        for (int pass = 0; pass < 2; pass++) {
            for (int i = 0; i < n; i++) keys[i] = i + 1;
            if (pass == 0)                                      /* random permutation */
                for (int i = n - 1; i > 0; i--) {
                    int j = (int)(next_rand() % (unsigned long long)(i + 1));
                    int tmp = keys[i]; keys[i] = keys[j]; keys[j] = tmp;
                }
            Bst t;
            bst_init(&t);
            double t0 = now_ms();
            for (int i = 0; i < n; i++) bst_insert(&t, keys[i]);
            double build = now_ms() - t0;
            long build_cmps = t.cmps;
            double avg = (double)bst_path_sum(&t) / n;
            double H = 0;
            for (int i = 1; i <= n; i++) H += 1.0 / i;
            double theory = pass == 0 ? 2 * (1 + 1.0 / n) * H - 3 : (n + 1) / 2.0;
            printf("%-8d %-12s %-8d %-14ld %-14.2f %-12.2f %-12.2f\n", n, pass == 0 ? "random" : "ascending",
                   bst_height(&t), build_cmps, avg, theory, build);
            bst_free(&t);
        }
        free(keys);
    }

    printf("\nAlternative: sorted array with binary search, n inserts in random order\n");
    printf("%-8s %-22s %-22s %-18s\n", "n", "BST build (ms)", "sorted-array build (ms)", "array avg search cmp");
    for (int si = 0; si < 3; si++) {
        int n = sizes[si];
        int *keys = malloc((size_t)n * sizeof *keys);
        int *arr = malloc((size_t)n * sizeof *arr);
        for (int i = 0; i < n; i++) keys[i] = i + 1;
        for (int i = n - 1; i > 0; i--) {
            int j = (int)(next_rand() % (unsigned long long)(i + 1));
            int tmp = keys[i]; keys[i] = keys[j]; keys[j] = tmp;
        }
        Bst t;
        bst_init(&t);
        double t0 = now_ms();
        for (int i = 0; i < n; i++) bst_insert(&t, keys[i]);
        double tb = now_ms() - t0;
        bst_free(&t);

        t0 = now_ms();
        int len = 0;
        for (int i = 0; i < n; i++) {
            int lo = 0, hi = len;
            while (lo < hi) { int mid = (lo + hi) / 2; if (arr[mid] < keys[i]) lo = mid + 1; else hi = mid; }
            memmove(arr + lo + 1, arr + lo, (size_t)(len - lo) * sizeof *arr);
            arr[lo] = keys[i];
            len++;
        }
        double ta = now_ms() - t0;
        long cmps = 0;
        for (int i = 0; i < n; i++) {
            int lo = 0, hi = n - 1;
            while (lo <= hi) {
                int mid = lo + (hi - lo) / 2;
                cmps++;
                if (arr[mid] == keys[i]) break;
                if (arr[mid] < keys[i]) lo = mid + 1; else hi = mid - 1;
            }
        }
        printf("%-8d %-22.2f %-22.2f %-18.2f\n", n, tb, ta, (double)cmps / n);
        free(keys); free(arr);
    }
    return 0;
}
