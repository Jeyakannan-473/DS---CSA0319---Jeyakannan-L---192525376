/* slotlist.h - ADT SlotList: ordered list of vehicle ids (array based) */
#ifndef SLOTLIST_H
#define SLOTLIST_H

#define SL_OK      0
#define SL_FULL   (-1)
#define SL_BADPOS (-2)
#define SL_EMPTY  (-3)

typedef struct {
    int *data;   /* storage, capacity ints          */
    int  size;   /* number of stored elements       */
    int  cap;    /* maximum number of elements      */
} SlotList;

/* pre : cap > 0                    post: size == 0, capacity == cap, returns SL_OK (or SL_FULL if malloc fails) */
int  sl_init(SlotList *L, int cap);
/* pre : L initialised              post: storage released, size == cap == 0 */
void sl_free(SlotList *L);
/* pre : 0 <= pos <= size           post: size' == size+1, L[pos] == val, elements pos.. shifted right
         errors: SL_FULL (size == cap, list unchanged), SL_BADPOS (pos < 0 or pos > size, list unchanged) */
int  sl_insert(SlotList *L, int pos, int val);
/* pre : 0 <= pos < size            post: size' == size-1, elements after pos shifted left, *out == removed value
         errors: SL_EMPTY (size == 0), SL_BADPOS (pos < 0 or pos >= size) */
int  sl_delete(SlotList *L, int pos, int *out);
/* pre : L initialised              post: list printed as [a, b, c]; list unchanged */
void sl_print(const SlotList *L);

#endif
