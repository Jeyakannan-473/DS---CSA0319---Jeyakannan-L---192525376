/* parking.h - Smart Parking Management System, design of Task 11 (version 1) */
#ifndef PARKING_H
#define PARKING_H
#include "hashmap.h"

#define PK_FULL      (-1)   /* park: no free slot            */
#define PK_DUPLICATE (-2)   /* park: vehicle already parked  */
#define PK_BADSLOT   (-3)   /* leave: slot id out of range   */
#define PK_EMPTYSLOT (-4)   /* leave: slot already free      */
#define PK_NOTFOUND  (-1)   /* lookup: vehicle not parked    */

typedef struct {
    int      nslots;
    int     *slot_vehicle;   /* slot_vehicle[s] = vehicle id, 0 = free          */
    int     *free_stack;     /* stack of free slot ids                          */
    int      top;            /* number of entries on free_stack                 */
    int      free_count;     /* maintained counter, invariant: == top           */
    HashMap  where;          /* vehicle id -> slot id                           */
} Parking;

int  pk_init(Parking *P, int nslots);          /* all slots free, free_count = nslots */
void pk_destroy(Parking *P);
int  pk_park(Parking *P, int vid);             /* slot id, PK_FULL or PK_DUPLICATE     */
int  pk_leave_slot(Parking *P, int slot);      /* vehicle id, PK_BADSLOT, PK_EMPTYSLOT */
int  pk_lookup(const Parking *P, int vid);     /* slot id or PK_NOTFOUND               */
int  pk_free_count(const Parking *P);          /* O(1)                                 */
int  pk_check(const Parking *P);               /* verifies invariants, 1 = consistent  */
#endif
