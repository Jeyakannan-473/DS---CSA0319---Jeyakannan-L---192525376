/* q3_disk.c - Task 3: height and disk-read calculation for an order-100
   B-tree against balanced binary trees, and the effect of the order. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "btree.h"

typedef struct PN { int key; struct PN *l, *r; } PN;

static PN *build_balanced(const int *a, int lo, int hi)
{
    if (lo > hi) return NULL;
    int mid = lo + (hi - lo) / 2;
    PN *p = malloc(sizeof *p);
    p->key = a[mid];
    p->l = build_balanced(a, lo, mid - 1);
    p->r = build_balanced(a, mid + 1, hi);
    return p;
}

static void free_pn(PN *p) { if (!p) return; free_pn(p->l); free_pn(p->r); free(p); }

int main(void)
{
    unsigned long long s = 88172645463325252ULL;
    long ns[] = {1000, 10000, 100000, 1000000};
    printf("Order-100 B-tree (t = 50) versus a balanced binary tree\n");
    printf("%-9s %-8s %-8s %-10s %-14s %-12s %-14s %-12s\n", "n", "levels", "nodes", "keys/node",
           "reads/search", "bound h<=", "binary reads", "log2(n+1)");
    for (int ni = 0; ni < 4; ni++) {
        long n = ns[ni];
        int *keys = malloc((size_t)n * sizeof *keys);
        for (long i = 0; i < n; i++) keys[i] = (int)(i + 1);
        for (long i = n - 1; i > 0; i--) {
            s ^= s << 13; s ^= s >> 7; s ^= s << 17;
            long j = (long)(s % (unsigned long long)(i + 1));
            int tmp = keys[i]; keys[i] = keys[j]; keys[j] = tmp;
        }
        BTree b;
        bt_init(&b, 50);
        for (long i = 0; i < n; i++) bt_insert(&b, keys[i]);
        b.reads = 0;
        int Q = 100000;
        for (int q = 0; q < Q; q++) {
            s ^= s << 13; s ^= s >> 7; s ^= s << 17;
            bt_search(&b, keys[s % (unsigned long long)n]);
        }
        double reads = (double)b.reads / Q;
        /* bound for t = 50: height in edges h <= log_t((n+1)/2), levels <= h + 1 */
        double bound = floor(log((double)(n + 1) / 2) / log(50.0)) + 1;

        /* perfectly balanced binary tree over the sorted keys */
        int *sorted = malloc((size_t)n * sizeof *sorted);
        for (long i = 0; i < n; i++) sorted[i] = (int)(i + 1);
        PN *root = build_balanced(sorted, 0, (int)n - 1);
        long visits = 0;
        for (int q = 0; q < Q; q++) {
            s ^= s << 13; s ^= s >> 7; s ^= s << 17;
            int key = (int)(s % (unsigned long long)n) + 1;
            PN *p = root;
            while (p) { visits++; if (key == p->key) break; p = key < p->key ? p->l : p->r; }
        }
        printf("%-9ld %-8d %-8zu %-10.1f %-14.2f %-12.0f %-14.2f %-12.2f  valid=%s\n", n, b.levels, b.nodes,
               (double)b.keys / (double)b.nodes, reads, bound, (double)visits / Q, log2((double)n + 1),
               bt_valid(&b) ? "yes" : "NO");
        free_pn(root); free(sorted); free(keys);
        bt_free(&b);
    }

    printf("\nEffect of the order on n = 1000000 random keys\n");
    printf("%-6s %-8s %-8s %-10s %-10s %-14s %-14s\n", "t", "order", "levels", "nodes", "keys/node", "reads/search", "node bytes");
    int ts[] = {2, 5, 10, 25, 50, 100};
    long n = 1000000;
    int *keys = malloc((size_t)n * sizeof *keys);
    for (long i = 0; i < n; i++) keys[i] = (int)(i + 1);
    for (long i = n - 1; i > 0; i--) {
        s ^= s << 13; s ^= s >> 7; s ^= s << 17;
        long j = (long)(s % (unsigned long long)(i + 1));
        int tmp = keys[i]; keys[i] = keys[j]; keys[j] = tmp;
    }
    for (int ti = 0; ti < 6; ti++) {
        BTree b;
        bt_init(&b, ts[ti]);
        for (long i = 0; i < n; i++) bt_insert(&b, keys[i]);
        b.reads = 0;
        int Q = 100000;
        for (int q = 0; q < Q; q++) {
            s ^= s << 13; s ^= s >> 7; s ^= s << 17;
            bt_search(&b, keys[s % (unsigned long long)n]);
        }
        /* bytes of one full node on disk: keys 4 B, child references 8 B, header 8 B */
        long node_bytes = 8 + (long)(2 * ts[ti] - 1) * 4 + (long)(2 * ts[ti]) * 8;
        printf("%-6d %-8d %-8d %-10zu %-10.1f %-14.2f %-14ld\n", ts[ti], 2 * ts[ti], b.levels, b.nodes,
               (double)b.keys / (double)b.nodes, (double)b.reads / Q, node_bytes);
        bt_free(&b);
    }
    free(keys);
    return 0;
}
