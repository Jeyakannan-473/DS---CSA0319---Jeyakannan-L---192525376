/* task4_arrays.c - array storage, address arithmetic, parameter decay */
#include <stdio.h>
static void decay(int arr[], int n)
{
    printf("  inside function : sizeof(arr) = %zu   (arr is really int*; n = %d)\n", sizeof(arr), n);
}
int main(void)
{
    int a[5] = {5, 10, 15, 20, 25};
    int *p = a;
    printf("Q1: %d %d %d\n", *p, *(p + 2), p[4]);
    p += 3;
    printf("Q2: %d %d\n", *p, *(p - 1));
    printf("Q3: %ld\n", (long)(&a[4] - &a[1]));
    printf("Q4: byte distance a[4]-a[1] = %ld\n", (long)((char *)&a[4] - (char *)&a[1]));
    printf("Q5: sizeof(a)=%zu  sizeof(int*)=%zu  elements=%zu\n", sizeof(a), sizeof(int *), sizeof(a) / sizeof(a[0]));
    printf("Q6: decay test\n  in main         : sizeof(a)   = %zu\n", sizeof(a));
    decay(a, 5);

    int m[3][4];
    for (int i = 0; i < 3; i++) for (int j = 0; j < 4; j++) m[i][j] = i * 4 + j;
    printf("Q7: m[2][1]=%d  *(*(m+1)+2)=%d  *(&m[0][0]+7)=%d\n", m[2][1], *(*(m + 1) + 2), *(&m[0][0] + 7));
    printf("Q8: sizeof(m)=%zu sizeof(m[0])=%zu sizeof(m[0][0])=%zu\n", sizeof(m), sizeof(m[0]), sizeof(m[0][0]));
    printf("Q9: byte offset of m[2][1] from m[0][0] = %ld (formula (2*4+1)*4 = %d)\n",
           (long)((char *)&m[2][1] - (char *)&m[0][0]), (2 * 4 + 1) * 4);
    /* flat vs 2-D vs per-level access for the parking lot : 8 floors x 10,000 slots */
    printf("Q10: flat index of (floor 5, slot 7321) = 5*10000+7321 = %d\n", 5 * 10000 + 7321);
    return 0;
}
