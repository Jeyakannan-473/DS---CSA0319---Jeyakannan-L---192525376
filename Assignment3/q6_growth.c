#define _POSIX_C_SOURCE 200809L
/* q6_growth.c - Task 6: growth of a 26-way trie with the number of words,
   against a sorted array with binary search and a linear scan. Words are
   2 to 4 consonant-vowel syllables, so many words share prefixes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "trie.h"

static unsigned long long rng = 88172645463325252ULL;
static unsigned long long next_rand(void)
{
    rng ^= rng << 13; rng ^= rng >> 7; rng ^= rng << 17;
    return rng;
}

static double now_ns(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec * 1e9 + (double)t.tv_nsec;
}

static void make_word(char *w)
{
    static const char on[] = "bdfgklmnprst", vo[] = "aeiou";
    int syl = 2 + (int)(next_rand() % 3), k = 0;
    for (int i = 0; i < syl; i++) {
        w[k++] = on[next_rand() % 12];
        w[k++] = vo[next_rand() % 5];
    }
    w[k] = '\0';
}

static long cmp_count;
static int cmp_str(const void *a, const void *b) { return strcmp((const char *)a, (const char *)b); }

static int bin_find(char (*arr)[16], int n, const char *w)
{
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        cmp_count++;
        int c = strcmp(arr[mid], w);
        if (c == 0) return mid;
        if (c < 0) lo = mid + 1; else hi = mid - 1;
    }
    return -1;
}

static volatile long sink;

int main(void)
{
    int sizes[] = {1000, 10000, 100000};
    printf("%-8s %-9s %-9s %-10s %-11s %-10s %-9s %-9s\n", "words", "avg len", "nodes", "nodes/word",
           "trie MB", "bytes/word", "build ms", "LCRS MB");
    static char (*sorted)[16];
    for (int si = 0; si < 3; si++) {
        int n = sizes[si];
        rng = 88172645463325252ULL;
        Trie t;
        trie_init(&t);
        char (*words)[16] = malloc((size_t)n * sizeof *words);
        long chars = 0;
        double t0 = now_ns();
        for (int i = 0; i < n;) {
            char w[16];
            make_word(w);
            if (trie_insert(&t, w) == 1) { strcpy(words[i], w); chars += (long)strlen(w); i++; }
        }
        double build = (now_ns() - t0) / 1e6;
        printf("%-8d %-9.2f %-9zu %-10.2f %-11.2f %-10.1f %-9.2f %-9.2f\n", n, (double)chars / n, t.nodes,
               (double)t.nodes / n, (double)trie_bytes(&t) / 1e6, (double)trie_bytes(&t) / n, build,
               (double)t.nodes * 24 / 1e6);
        trie_free(&t);
        free(words);
    }

    printf("\nLook-up and autocomplete cost\n");
    printf("%-8s %-12s %-12s %-12s %-12s %-12s %-14s %-14s\n", "words", "trie steps", "trie ns", "bsearch cmp",
           "bsearch ns", "linear cmp", "trie ac (ns)", "array ac (ns)");
    for (int si = 0; si < 3; si++) {
        int n = sizes[si];
        rng = 88172645463325252ULL;
        Trie t;
        trie_init(&t);
        char (*words)[16] = malloc((size_t)n * sizeof *words);
        for (int i = 0; i < n;) {
            char w[16];
            make_word(w);
            if (trie_insert(&t, w) == 1) { strcpy(words[i], w); i++; }
        }
        sorted = malloc((size_t)n * sizeof *sorted);
        memcpy(sorted, words, (size_t)n * sizeof *words);
        qsort(sorted, (size_t)n, sizeof *sorted, cmp_str);
        int Q = 200000;
        int *qi = malloc((size_t)Q * sizeof *qi);
        for (int i = 0; i < Q; i++) qi[i] = (int)(next_rand() % (unsigned long long)n);

        t.steps = 0;
        double t0 = now_ns();
        long acc = 0;
        for (int i = 0; i < Q; i++) acc += trie_search(&t, words[qi[i]]);
        double trie_ns = (now_ns() - t0) / Q;
        double trie_steps = (double)t.steps / Q;
        sink += acc;

        cmp_count = 0;
        t0 = now_ns();
        acc = 0;
        for (int i = 0; i < Q; i++) acc += bin_find(sorted, n, words[qi[i]]);
        double bs_ns = (now_ns() - t0) / Q;
        double bs_cmp = (double)cmp_count / Q;
        sink += acc;

        long lin = 0;
        int LQ = n <= 10000 ? 1000 : 100;
        for (int i = 0; i < LQ; i++)
            for (int j = 0; j < n; j++) { lin++; if (strcmp(words[j], words[qi[i]]) == 0) break; }

        /* autocomplete: first 10 words for 3-letter prefixes */
        int AQ = 20000;
        char out[10][TRIE_MAXW];
        t0 = now_ns();
        long got = 0;
        for (int i = 0; i < AQ; i++) {
            char pre[4];
            memcpy(pre, words[qi[i]], 3); pre[3] = '\0';
            got += trie_autocomplete(&t, pre, 10, out);
        }
        double ac_trie = (now_ns() - t0) / AQ;
        t0 = now_ns();
        long got2 = 0;
        for (int i = 0; i < AQ; i++) {
            char pre[4];
            memcpy(pre, words[qi[i]], 3); pre[3] = '\0';
            int lo = 0, hi = n;
            while (lo < hi) { int mid = (lo + hi) / 2; if (strncmp(sorted[mid], pre, 3) < 0) lo = mid + 1; else hi = mid; }
            for (int k = lo; k < n && k < lo + 10 && strncmp(sorted[k], pre, 3) == 0; k++) got2++;
        }
        double ac_arr = (now_ns() - t0) / AQ;
        if (got != got2) printf("ERROR: autocomplete results differ (%ld vs %ld)\n", got, got2);
        printf("%-8d %-12.2f %-12.1f %-12.2f %-12.1f %-12.1f %-14.1f %-14.1f\n", n, trie_steps, trie_ns, bs_cmp, bs_ns,
               (double)lin / LQ, ac_trie, ac_arr);
        free(sorted); free(words); free(qi);
        trie_free(&t);
    }
    return 0;
}
