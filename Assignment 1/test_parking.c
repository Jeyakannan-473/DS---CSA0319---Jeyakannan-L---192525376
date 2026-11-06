/* test_parking.c - boundary tests and 10-event sequence for Task 9 */
#include <stdio.h>
#include "parking.h"

static int pass = 0, total = 0;
static void check(const char *what, int expected, int actual)
{
    total++; int ok = (expected == actual); pass += ok;
    printf("[%s] %-48s expected=%3d actual=%3d\n", ok ? "PASS" : "FAIL", what, expected, actual);
}

int main(void)
{
    Parking P;
    printf("=== A. empty lot (5 slots) ===\n");
    pk_init(&P, 5);
    check("free_count after init", 5, pk_free_count(&P));
    check("leave on a free slot -> PK_EMPTYSLOT", PK_EMPTYSLOT, pk_leave_slot(&P, 2));
    check("lookup in empty lot -> PK_NOTFOUND", PK_NOTFOUND, pk_lookup(&P, 111));
    check("leave slot -1 -> PK_BADSLOT", PK_BADSLOT, pk_leave_slot(&P, -1));
    check("leave slot 5 (== nslots) -> PK_BADSLOT", PK_BADSLOT, pk_leave_slot(&P, 5));

    printf("=== B. full lot ===\n");
    for (int v = 1; v <= 5; v++) pk_park(&P, 100 + v);
    check("free_count when full", 0, pk_free_count(&P));
    check("park into full lot -> PK_FULL", PK_FULL, pk_park(&P, 999));
    check("invariants hold when full", 1, pk_check(&P));

    printf("=== C. first and last slot ===\n");
    check("first slot (0) holds vehicle 101", 101, pk_leave_slot(&P, 0));
    check("last slot (4) holds vehicle 105", 105, pk_leave_slot(&P, 4));
    check("lookup of left vehicle 101 -> PK_NOTFOUND", PK_NOTFOUND, pk_lookup(&P, 101));
    check("re-park: last freed slot (4) is reused first", 4, pk_park(&P, 777));
    check("duplicate park of 777 -> PK_DUPLICATE", PK_DUPLICATE, pk_park(&P, 777));
    check("free_count", 1, pk_free_count(&P));
    check("invariants hold", 1, pk_check(&P));
    pk_destroy(&P);

    printf("=== D. ten-event sequence (4-slot lot) ===\n");
    pk_init(&P, 4);
    check("E1  park 101           -> slot", 0, pk_park(&P, 101));
    check("E2  park 102           -> slot", 1, pk_park(&P, 102));
    check("E3  park 103           -> slot", 2, pk_park(&P, 103));
    check("E4  leave slot 1       -> vehicle", 102, pk_leave_slot(&P, 1));
    check("E5  park 104           -> slot (reuses 1)", 1, pk_park(&P, 104));
    check("E6  park 105           -> slot", 3, pk_park(&P, 105));
    check("E7  park 106 (lot full)-> PK_FULL", PK_FULL, pk_park(&P, 106));
    check("E8  lookup 103         -> slot", 2, pk_lookup(&P, 103));
    check("E9  leave slot 0       -> vehicle", 101, pk_leave_slot(&P, 0));
    check("E10 lookup 101         -> PK_NOTFOUND", PK_NOTFOUND, pk_lookup(&P, 101));
    check("final free_count", 1, pk_free_count(&P));
    check("final invariants", 1, pk_check(&P));
    pk_destroy(&P);
    printf("\nSummary: %d / %d tests passed\n", pass, total);
    return pass == total ? 0 : 1;
}
