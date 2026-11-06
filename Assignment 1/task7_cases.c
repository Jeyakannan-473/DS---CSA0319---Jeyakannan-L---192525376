/* task7_cases.c - best / average / worst case counts for slot-record search */
#include <stdio.h>
static long cmp;
static int linear(const int *a, int n, int key){ for(int i=0;i<n;i++){ cmp++; if(a[i]==key) return i; } return -1; }
static int binary(const int *a, int n, int key){
    int lo=0, hi=n-1;
    while(lo<=hi){ int mid=(lo+hi)/2; cmp++; if(a[mid]==key) return mid; if(a[mid]<key) lo=mid+1; else hi=mid-1; }
    return -1; }
int main(void)
{
    enum { N = 1023 };
    static int a[N];
    for (int i = 0; i < N; i++) a[i] = 2 * i + 1;            /* sorted odd numbers */
    long best, worst, tot = 0;
    cmp = 0; linear(a, N, a[0]);     best = cmp;
    cmp = 0; linear(a, N, a[N-1]);   worst = cmp;
    for (int i = 0; i < N; i++) { cmp = 0; linear(a, N, a[i]); tot += cmp; }
    printf("n=%d LINEAR  best=%ld worst=%ld average(successful)=%.1f (n+1)/2=%.1f\n", N, best, worst, (double)tot/N, (N+1)/2.0);
    cmp = 0; linear(a, N, 2);        printf("n=%d LINEAR  unsuccessful search = %ld comparisons\n", N, cmp);
    tot = 0; long mx = 0;
    for (int i = 0; i < N; i++) { cmp = 0; binary(a, N, a[i]); tot += cmp; if (cmp > mx) mx = cmp; }
    cmp = 0; binary(a, N, a[(N-1)/2]);
    printf("n=%d BINARY  best=%ld worst=%ld average(successful)=%.3f  (log2(n+1)=10)\n", N, cmp, mx, (double)tot/N);
    int u[8] = {7, 1, 9, 3, 5, 2, 8, 4};                      /* unsorted data breaks binary search */
    cmp = 0; int r = binary(u, 8, 2);
    printf("binary search for 2 in UNSORTED array {7,1,9,3,5,2,8,4}: returned %d (2 is really at index 5) -> wrong\n", r);
    return 0;
}
