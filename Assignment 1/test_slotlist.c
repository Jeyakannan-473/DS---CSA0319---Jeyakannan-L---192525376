/* test_slotlist.c - boundary tests for the SlotList ADT */
#include <stdio.h>
#include "slotlist.h"

static int pass = 0, total = 0;
static void check(const char *what, int expected, int actual)
{
    total++;
    int ok = (expected == actual);
    pass += ok;
    printf("[%s] %-46s expected=%2d actual=%2d\n", ok ? "PASS" : "FAIL", what, expected, actual);
}

int main(void)
{
    SlotList L; int out = -99;
    sl_init(&L, 5);
    printf("--- empty-list tests ---\n");
    check("delete from empty list -> SL_EMPTY", SL_EMPTY, sl_delete(&L, 0, &out));
    check("insert at pos 1 in empty list -> SL_BADPOS", SL_BADPOS, sl_insert(&L, 1, 7));
    check("insert at pos -1 -> SL_BADPOS", SL_BADPOS, sl_insert(&L, -1, 7));
    printf("--- normal inserts ---\n");
    check("insert 10 at pos 0 (empty)", SL_OK, sl_insert(&L, 0, 10));
    check("insert 30 at end (pos 1)", SL_OK, sl_insert(&L, 1, 30));
    check("insert 20 in middle (pos 1)", SL_OK, sl_insert(&L, 1, 20));
    check("insert 5 at front (pos 0)", SL_OK, sl_insert(&L, 0, 5));
    printf("list = "); sl_print(&L); printf("   (expected [5, 10, 20, 30])\n");
    check("insert 40 at end -> list now full", SL_OK, sl_insert(&L, 4, 40));
    printf("--- full-list tests ---\n");
    check("insert into full list -> SL_FULL", SL_FULL, sl_insert(&L, 0, 99));
    check("size unchanged after failed insert", 5, L.size);
    printf("--- invalid-position deletes ---\n");
    check("delete pos 5 (== size) -> SL_BADPOS", SL_BADPOS, sl_delete(&L, 5, &out));
    check("delete pos -1 -> SL_BADPOS", SL_BADPOS, sl_delete(&L, -1, &out));
    printf("--- deletes: first, middle, last ---\n");
    check("delete first (pos 0)", SL_OK, sl_delete(&L, 0, &out));  check("  removed value", 5, out);
    check("delete middle (pos 1)", SL_OK, sl_delete(&L, 1, &out)); check("  removed value", 20, out);
    check("delete last (pos 2)", SL_OK, sl_delete(&L, 2, &out));   check("  removed value", 40, out);
    printf("list = "); sl_print(&L); printf("   (expected [10, 30])\n");
    check("size after three deletes", 2, L.size);
    sl_free(&L);
    printf("\nSummary: %d / %d tests passed\n", pass, total);
    return pass == total ? 0 : 1;
}
