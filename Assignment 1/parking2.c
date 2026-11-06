#include <stdlib.h>
#include "parking2.h"

static void h_push(MinHeap *h, int v)                 /* O(log n) sift-up */
{
    int i = h->size++;
    while (i > 0 && h->a[(i - 1) / 2] > v) { h->a[i] = h->a[(i - 1) / 2]; i = (i - 1) / 2; }
    h->a[i] = v;
}
static int h_pop(MinHeap *h)                          /* O(log n) sift-down; caller checks size > 0 */
{
    int top = h->a[0], v = h->a[--h->size], i = 0;
    while (2 * i + 1 < h->size) {
        int c = 2 * i + 1;
        if (c + 1 < h->size && h->a[c + 1] < h->a[c]) c++;
        if (h->a[c] >= v) break;
        h->a[i] = h->a[c]; i = c;
    }
    if (h->size > 0) h->a[i] = v;
    return top;
}

int pk2_init(Parking2 *P, int nslots, int n_reserved, int ev_stride)
{
    P->nslots = nslots; P->n_reserved = n_reserved; P->ev_stride = ev_stride; P->next_reserved = 0;
    P->type = (unsigned char *)calloc((size_t)nslots, 1);
    P->slot_vehicle = (int *)calloc((size_t)nslots, sizeof(int));
    P->normal.a = (int *)malloc((size_t)nslots * sizeof(int)); P->normal.size = 0;
    P->ev.a     = (int *)malloc((size_t)nslots * sizeof(int)); P->ev.size = 0;
    if (!P->type || !P->slot_vehicle || !P->normal.a || !P->ev.a) return -1;
    for (int s = 0; s < nslots; s++) {                 /* ascending order => already a valid min-heap: O(n) build */
        if (s < n_reserved)                          P->type[s] = 2;
        else if (ev_stride > 0 && s % ev_stride == ev_stride - 1) { P->type[s] = 1; P->ev.a[P->ev.size++] = s; }
        else                                         P->normal.a[P->normal.size++] = s;
    }
    P->free_count = nslots;
    int nb = (int)((nslots / 0.75) + 1);
    if (hm_init(&P->where, nb)) return -1;
    return hm_init(&P->pass, n_reserved > 0 ? n_reserved * 2 + 1 : 1);
}

void pk2_destroy(Parking2 *P)
{
    free(P->type); free(P->slot_vehicle); free(P->normal.a); free(P->ev.a); hm_free(&P->where); hm_free(&P->pass);
}

int pk2_register_pass(Parking2 *P, int vid)
{
    if (hm_get(&P->pass, vid) != -1) return -2;
    if (P->next_reserved >= P->n_reserved) return -1;
    int s = P->next_reserved++;
    hm_put(&P->pass, vid, s);
    return s;
}

int pk2_park(Parking2 *P, int vid, int want_ev)
{
    if (hm_get(&P->where, vid) != -1) return PK_DUPLICATE;
    int s = -1, ps = hm_get(&P->pass, vid);
    if (ps >= 0 && P->slot_vehicle[ps] == 0)       s = ps;                       /* monthly pass: own slot, O(1) */
    else if (want_ev && P->ev.size > 0)            s = h_pop(&P->ev);            /* nearest free EV slot        */
    else if (P->normal.size > 0)                   s = h_pop(&P->normal);        /* nearest free normal slot    */
    else return PK_FULL;
    P->slot_vehicle[s] = vid;
    hm_put(&P->where, vid, s);
    P->free_count--;
    return s;
}

int pk2_leave_slot(Parking2 *P, int slot)
{
    if (slot < 0 || slot >= P->nslots) return PK_BADSLOT;
    int vid = P->slot_vehicle[slot];
    if (vid == 0) return PK_EMPTYSLOT;
    P->slot_vehicle[slot] = 0;
    hm_del(&P->where, vid);
    if (P->type[slot] == 1) h_push(&P->ev, slot);
    else if (P->type[slot] == 0) h_push(&P->normal, slot);
    P->free_count++;
    return vid;
}

int pk2_lookup(const Parking2 *P, int vid) { return hm_get(&P->where, vid); }

int pk2_check(const Parking2 *P)
{
    int zeros = 0, free_res = 0;
    for (int s = 0; s < P->nslots; s++) {
        if (P->slot_vehicle[s] == 0) { zeros++; if (P->type[s] == 2) free_res++; }
        else if (hm_get(&P->where, P->slot_vehicle[s]) != s) return 0;
    }
    return zeros == P->free_count && zeros == P->normal.size + P->ev.size + free_res;
}
