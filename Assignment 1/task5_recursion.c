/* task5_recursion.c - recursive solutions, call counts, depth, frame size */
#include <stdio.h>
static int calls, depth, maxdepth;
static void show(int n)               /* prints 1..n */
{
    if (n == 0) return;
    show(n - 1);
    printf("%d ", n);
}
static int mystery(int n)             /* counts the 1-bits of n */
{
    calls++; depth++; if (depth > maxdepth) maxdepth = depth;
    int r = (n == 0) ? 0 : mystery(n / 2) + (n % 2);
    depth--; return r;
}
static int count_lin(const int *a, int n)          /* linear recursion */
{
    calls++; depth++; if (depth > maxdepth) maxdepth = depth;
    int r = (n == 0) ? 0 : (a[n - 1] == 0) + count_lin(a, n - 1);
    depth--; return r;
}
static int count_split(const int *a, int lo, int hi)   /* divide and conquer */
{
    calls++; depth++; if (depth > maxdepth) maxdepth = depth;
    int r;
    if (lo == hi) r = (a[lo] == 0);
    else { int mid = (lo + hi) / 2; r = count_split(a, lo, mid) + count_split(a, mid + 1, hi); }
    depth--; return r;
}
static int count_iter(const int *a, int n) { int c = 0; for (int i = 0; i < n; i++) c += (a[i] == 0); return c; }


int main(void)
{
    printf("show(5)    prints: "); show(5); printf("\n");
    calls = depth = maxdepth = 0;
    int mr = mystery(13);
    printf("mystery(13) = %d  (calls=%d, max depth=%d)\n", mr, calls, maxdepth);
    int slots[10] = {0, 7, 0, 0, 9, 0, 3, 0, 0, 4};     /* 0 = free: 6 free slots */
    calls = depth = maxdepth = 0;
    int r1 = count_lin(slots, 10);   printf("count_free linear : result=%d calls=%d max depth=%d\n", r1, calls, maxdepth);
    calls = depth = maxdepth = 0;
    int r2 = count_split(slots, 0, 9); printf("count_free split  : result=%d calls=%d max depth=%d\n", r2, calls, maxdepth);
    printf("count_free loop   : result=%d (0 extra calls, 1 stack frame)\n", count_iter(slots, 10));
    for (int n = 1; n <= 16; n *= 2) {
        static int z[16];
        calls = depth = maxdepth = 0; count_lin(z, n); int c1 = calls, d1 = maxdepth;
        calls = depth = maxdepth = 0; count_split(z, 0, n - 1);
        printf("n=%2d  linear: calls=%2d depth=%2d | split: calls=%2d depth=%d\n", n, c1, d1, calls, maxdepth);
    }
    return 0;
}
