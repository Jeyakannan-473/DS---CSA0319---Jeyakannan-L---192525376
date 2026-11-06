/* parking2.h - revised design (Task 12): nearest-slot allocation, EV slots, monthly-pass reserved slots */
#ifndef PARKING2_H
#define PARKING2_H
#include "hashmap.h"
#include "parking.h"            /* reuse PK_* result codes */

typedef struct { int *a; int size; } MinHeap;     /* binary min-heap of free slot ids (id = distance from entrance) */

typedef struct {
    int nslots, n_reserved, ev_stride, next_reserved;
    unsigned char *type;        /* 0 = normal, 1 = EV charging, 2 = reserved for monthly pass */
    int     *slot_vehicle;      /* 0 = free                                                   */
    MinHeap  normal, ev;        /* free normal slots, free EV slots (reserved slots not in a heap) */
    HashMap  where;             /* vehicle -> slot                                            */
    HashMap  pass;              /* pass holder -> reserved slot                               */
    int      free_count;
} Parking2;

int  pk2_init(Parking2 *P, int nslots, int n_reserved, int ev_stride);
void pk2_destroy(Parking2 *P);
int  pk2_register_pass(Parking2 *P, int vid);        /* reserved slot id, -1 none left, -2 already holder */
int  pk2_park(Parking2 *P, int vid, int want_ev);    /* slot id or PK_FULL / PK_DUPLICATE                 */
int  pk2_leave_slot(Parking2 *P, int slot);
int  pk2_lookup(const Parking2 *P, int vid);
int  pk2_check(const Parking2 *P);
#endif
