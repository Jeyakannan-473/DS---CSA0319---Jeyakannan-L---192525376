/* test_parking2.c - tests of the revised design (EV, nearest slot, monthly pass) */
#include <stdio.h>
#include "parking2.h"
static int pass = 0, total = 0;
static void check(const char *what, int expected, int actual)
{
    total++; int ok = (expected == actual); pass += ok;
    printf("[%s] %-56s expected=%3d actual=%3d\n", ok ? "PASS" : "FAIL", what, expected, actual);
}
int main(void)
{
    Parking2 P;
    pk2_init(&P, 12, 2, 4);   /* slots 0,1 reserved; EV slots 3,7,11; normal 2,4,5,6,8,9,10 */
    printf("layout: reserved {0,1}  EV {3,7,11}  normal {2,4,5,6,8,9,10}\n");
    check("register pass 501 -> reserved slot", 0, pk2_register_pass(&P, 501));
    check("register pass 502 -> reserved slot", 1, pk2_register_pass(&P, 502));
    check("register pass 503 -> none left (-1)", -1, pk2_register_pass(&P, 503));
    check("register pass 501 again -> already holder (-2)", -2, pk2_register_pass(&P, 501));
    check("normal car 201 gets nearest NORMAL slot", 2, pk2_park(&P, 201, 0));
    check("normal car 202 skips EV slot 3, gets 4", 4, pk2_park(&P, 202, 0));
    check("EV car 301 gets nearest EV slot", 3, pk2_park(&P, 301, 1));
    check("pass holder 501 gets reserved slot 0", 0, pk2_park(&P, 501, 0));
    check("normal car 204 gets slot 5", 5, pk2_park(&P, 204, 0));
    check("leave slot 2 -> vehicle 201", 201, pk2_leave_slot(&P, 2));
    check("leave slot 4 -> vehicle 202", 202, pk2_leave_slot(&P, 4));
    check("car 205: NEAREST free slot is 2 (v1 stack would give 4)", 2, pk2_park(&P, 205, 0));
    check("EV car 302 gets EV slot 7", 7, pk2_park(&P, 302, 1));
    check("EV car 303 gets EV slot 11", 11, pk2_park(&P, 303, 1));
    check("EV car 304 (no EV slot left) falls back to normal 4", 4, pk2_park(&P, 304, 1));
    check("normal car 206 -> 6", 6, pk2_park(&P, 206, 0));
    check("normal car 207 -> 8", 8, pk2_park(&P, 207, 0));
    check("normal car 208 -> 9", 9, pk2_park(&P, 208, 0));
    check("normal car 209 -> 10", 10, pk2_park(&P, 209, 0));
    check("normal car 210: no normal slot -> PK_FULL", PK_FULL, pk2_park(&P, 210, 0));
    check("free_count before 502 parks (reserved slot 1 is free)", 1, P.free_count);
    check("pass holder 502 parks in reserved slot 1", 1, pk2_park(&P, 502, 0));
    check("free_count when completely full", 0, P.free_count);
    check("invariants hold", 1, pk2_check(&P));
    check("leave EV slot 3 -> vehicle 301", 301, pk2_leave_slot(&P, 3));
    check("non-EV car 211 may NOT take the free EV slot", PK_FULL, pk2_park(&P, 211, 0));
    check("EV car 305 takes EV slot 3", 3, pk2_park(&P, 305, 1));
    check("lookup 305 -> slot", 3, pk2_lookup(&P, 305));
    check("invariants hold at end", 1, pk2_check(&P));
    pk2_destroy(&P);
    printf("\nSummary: %d / %d tests passed\n", pass, total);
    return pass == total ? 0 : 1;
}
