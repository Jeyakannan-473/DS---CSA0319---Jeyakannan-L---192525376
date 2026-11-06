/* task8_space.c - sizeof confirmation: int array vs bitmap, stack space of recursion */
#include <stdio.h>
#include <string.h>
#define N 80000
static int  slots_int[N];
static unsigned char slots_bit[(N + 7) / 8];
#define SETBIT(b,i)  ((b)[(i)>>3] |=  (unsigned char)(1u << ((i)&7)))
#define CLRBIT(b,i)  ((b)[(i)>>3] &= (unsigned char)~(1u << ((i)&7)))
#define TSTBIT(b,i)  (((b)[(i)>>3] >> ((i)&7)) & 1)
static long maxd, d; static long first, last;
static int rec_count(const unsigned char *b, int n){ char m; d++; if(d>maxd) maxd=d; if(d==1) first=(long)&m; last=(long)&m;
    int r = (n==0)?0: TSTBIT(b,n-1) + rec_count(b,n-1); d--; return r; }
int main(void)
{
    printf("sizeof(int)                      = %zu bytes\n", sizeof(int));
    printf("sizeof(int[80000])               = %zu bytes\n", sizeof(slots_int));
    printf("sizeof(unsigned char[10000])     = %zu bytes (bitmap, 1 bit per slot)\n", sizeof(slots_bit));
    printf("ratio int array / bitmap         = %.0f\n", (double)sizeof(slots_int) / sizeof(slots_bit));
    memset(slots_bit, 0, sizeof slots_bit);
    for (int i = 0; i < N; i += 3) SETBIT(slots_bit, i);       /* occupy every 3rd slot */
    SETBIT(slots_bit, 79999); CLRBIT(slots_bit, 0);
    int occ = 0; for (int i = 0; i < N; i++) occ += TSTBIT(slots_bit, i);
    printf("bitmap test: occupied=%d (expected 26667 - 1 + 1 = 26667), slot 3 -> %d, slot 4 -> %d, slot 79999 -> %d\n",
           occ, TSTBIT(slots_bit,3), TSTBIT(slots_bit,4), TSTBIT(slots_bit,79999));
    int r = rec_count(slots_bit, N);
    printf("recursive count over 80000 slots: result=%d, max depth=%ld frames, stack used ~ %ld bytes (%.1f bytes/frame)\n",
           r, maxd, first - last, (double)(first - last) / (maxd - 1));
    return 0;
}
