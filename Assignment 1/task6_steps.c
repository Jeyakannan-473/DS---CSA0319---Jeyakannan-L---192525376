/* task6_steps.c - step counting for four loops, growth table, maintained free count */
#include <stdio.h>
static long loop1(long n){ long s=0; for(long i=1;i<=n;i++) for(long j=1;j<=i;j++) s++; return s; }
static long loop2(long n){ long s=0; for(long i=1;i<n;i*=2) s++; return s; }
static long loop3(long n){ long s=0; for(long i=0;i<n;i++) for(long j=1;j<n;j*=2) s++; return s; }
static long loop4(long n){ long s=0; for(long i=1;i*i<=n;i++) s++; return s; }
int main(void)
{
    long ns[] = {16, 200, 2000, 80000};
    printf("%8s %16s %6s %12s %6s\n", "n", "L1 n(n+1)/2", "L2", "L3 n*log n", "L4");
    for (int k = 0; k < 4; k++)
        printf("%8ld %16ld %6ld %12ld %6ld\n", ns[k], loop1(ns[k]), loop2(ns[k]), loop3(ns[k]), loop4(ns[k]));
    return 0;
}
