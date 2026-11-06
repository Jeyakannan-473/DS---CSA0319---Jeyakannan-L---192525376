#include <stdlib.h>
#include "parking.h"

int pk_init(Parking *P, int nslots)
{
    P->nslots = nslots;
    P->slot_vehicle = (int *)calloc((size_t)nslots, sizeof(int));
    P->free_stack   = (int *)malloc((size_t)nslots * sizeof(int));
    if (!P->slot_vehicle || !P->free_stack) return -1;
    for (int i = 0; i < nslots; i++) P->free_stack[i] = nslots - 1 - i;   /* slot 0 on top */
    P->top = nslots;
    P->free_count = nslots;
    int nb = (int)((nslots / 0.75) + 1);                                  /* load factor <= 0.75 */
    return hm_init(&P->where, nb);
}

void pk_destroy(Parking *P)
{
    free(P->slot_vehicle); free(P->free_stack); hm_free(&P->where);
}

int pk_park(Parking *P, int vid)
{
    if (P->top == 0) return PK_FULL;
    if (hm_get(&P->where, vid) != -1) return PK_DUPLICATE;
    int s = P->free_stack[--P->top];       /* O(1) pop */
    P->slot_vehicle[s] = vid;
    hm_put(&P->where, vid, s);             /* O(1) expected */
    P->free_count--;
    return s;
}

int pk_leave_slot(Parking *P, int slot)
{
    if (slot < 0 || slot >= P->nslots) return PK_BADSLOT;
    int vid = P->slot_vehicle[slot];
    if (vid == 0) return PK_EMPTYSLOT;
    P->slot_vehicle[slot] = 0;
    hm_del(&P->where, vid);
    P->free_stack[P->top++] = slot;        /* O(1) push */
    P->free_count++;
    return vid;
}

int pk_lookup(const Parking *P, int vid) { return hm_get(&P->where, vid); }
int pk_free_count(const Parking *P)      { return P->free_count; }

int pk_check(const Parking *P)
{
    int zeros = 0;
    for (int s = 0; s < P->nslots; s++) {
        if (P->slot_vehicle[s] == 0) zeros++;
        else if (hm_get(&P->where, P->slot_vehicle[s]) != s) return 0;
    }
    return zeros == P->free_count && P->free_count == P->top && P->where.count == P->nslots - P->free_count;
}
