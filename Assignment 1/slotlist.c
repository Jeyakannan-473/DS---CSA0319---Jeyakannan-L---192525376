/* slotlist.c - array implementation of the SlotList ADT */
#include <stdio.h>
#include <stdlib.h>
#include "slotlist.h"

int sl_init(SlotList *L, int cap)
{
    L->data = (int *)malloc((size_t)cap * sizeof(int));
    L->size = 0;
    L->cap  = cap;
    return L->data ? SL_OK : SL_FULL;
}

void sl_free(SlotList *L)
{
    free(L->data);
    L->data = NULL;
    L->size = L->cap = 0;
}

int sl_insert(SlotList *L, int pos, int val)
{
    if (L->size == L->cap)         return SL_FULL;
    if (pos < 0 || pos > L->size)  return SL_BADPOS;
    for (int i = L->size; i > pos; i--)     /* shift right: n-pos steps */
        L->data[i] = L->data[i - 1];
    L->data[pos] = val;
    L->size++;
    return SL_OK;
}

int sl_delete(SlotList *L, int pos, int *out)
{
    if (L->size == 0)              return SL_EMPTY;
    if (pos < 0 || pos >= L->size) return SL_BADPOS;
    if (out) *out = L->data[pos];
    for (int i = pos; i < L->size - 1; i++) /* shift left: n-pos-1 steps */
        L->data[i] = L->data[i + 1];
    L->size--;
    return SL_OK;
}

void sl_print(const SlotList *L)
{
    printf("[");
    for (int i = 0; i < L->size; i++)
        printf(i ? ", %d" : "%d", L->data[i]);
    printf("]");
}
