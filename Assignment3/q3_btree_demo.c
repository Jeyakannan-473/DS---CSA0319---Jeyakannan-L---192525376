/* q3_btree_demo.c - Task 3: B-tree insertion with split trace (t = 2, order 4),
   search tests and boundary tests, then validation on larger trees. */
#include <stdio.h>
#include <string.h>
#include "btree.h"

int main(void)
{
    BTree tr;
    bt_init(&tr, 2);
    int keys[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 25, 35, 45, 55};
    int n = (int)(sizeof keys / sizeof keys[0]);
    printf("PART A - insertion trace, minimum degree t = 2 (order 4, at most 3 keys per node)\n");
    for (int i = 0; i < n; i++) {
        long sp = tr.splits;
        bt_insert(&tr, keys[i]);
        printf("insert %-3d splits this insert = %ld, total splits = %ld, levels = %d, nodes = %zu\n",
               keys[i], tr.splits - sp, tr.splits, tr.levels, tr.nodes);
        bt_print_levels(&tr);
    }
    printf("structure valid: %s\n", bt_valid(&tr) ? "yes" : "NO");

    printf("\nPART B - search tests (reads = nodes visited)\n");
    int probes[] = {40, 20, 80, 10, 100, 45, 5, 1000, 65, 0};
    const char *what[] = {"key in the root", "key on the second level", "key on the second level", "smallest key",
                          "largest key", "key in a leaf", "below the smallest", "above the largest",
                          "missing, inside the range", "zero"};
    for (int i = 0; i < 10; i++) {
        long before = tr.reads;
        int f = bt_search(&tr, probes[i]);
        printf("search %-5d (%-26s) -> %-9s reads = %ld\n", probes[i], what[i], f ? "found" : "not found", tr.reads - before);
    }

    printf("\nPART C - boundary tests\n");
    BTree e;
    bt_init(&e, 2);
    printf("empty tree: search(7) = %s, levels = %d, valid = %s\n", bt_search(&e, 7) ? "found" : "not found",
           e.levels, bt_valid(&e) ? "yes" : "NO");
    int r1 = bt_insert(&e, 7);
    int r2 = bt_insert(&e, 7);
    printf("insert 7 -> %d, insert 7 again -> %d (duplicate rejected), keys = %zu, nodes = %zu\n", r1, r2, e.keys, e.nodes);
    bt_free(&e);
    bt_free(&tr);

    printf("\nPART D - validation on larger trees\n");
    int ts[] = {2, 3, 50};
    unsigned long long s = 88172645463325252ULL;
    for (int ti = 0; ti < 3; ti++) {
        for (int pass = 0; pass < 2; pass++) {
            BTree b;
            bt_init(&b, ts[ti]);
            int N = 20000, accepted = 0;
            for (int i = 1; i <= N; i++) {
                int k = i;
                if (pass == 0) { s ^= s << 13; s ^= s >> 7; s ^= s << 17; k = (int)(s % 1000003ULL); }
                accepted += bt_insert(&b, k);
            }
            int missing = 0;
            for (int i = 0; i < 2000; i++) {
                s ^= s << 13; s ^= s >> 7; s ^= s << 17;
                int k = (int)(s % 1000003ULL);
                int f = bt_search(&b, k);
                if (pass == 1 && f != (k >= 1 && k <= N)) missing++;
            }
            printf("t = %-3d (order %-3d) %-10s keys = %-6zu levels = %-2d nodes = %-5zu splits = %-5ld valid = %s%s\n",
                   ts[ti], 2 * ts[ti], pass == 0 ? "random" : "ascending", b.keys, b.levels, b.nodes, b.splits,
                   bt_valid(&b) ? "yes" : "NO", missing ? " SEARCH ERRORS" : "");
            (void)accepted;
            bt_free(&b);
        }
    }
    return 0;
}
