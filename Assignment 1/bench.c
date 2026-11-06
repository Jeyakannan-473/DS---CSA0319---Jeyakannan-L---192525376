/* bench.c - Task 10 / 11 / 12 measurements. Build: gcc -O2 -o bench bench.c parking.c parking2.c hashmap.c slotlist.c
   Method: clock() around many repetitions, 5 runs per point, median reported (ns per operation). */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "parking.h"
#include "parking2.h"
#include "slotlist.h"

static volatile long sink;
#define NOINL __attribute__((noinline))
NOINL static int scan_count_free(const int *a, int n){ int c=0; for(int i=0;i<n;i++) c += (a[i]==0); return c; }
NOINL static int scan_first_free(const int *a, int n){ for(int i=0;i<n;i++) if(a[i]==0) return i; return -1; }
NOINL static int lin_search(const int *a, int n, int key){ for(int i=0;i<n;i++) if(a[i]==key) return i; return -1; }
NOINL static int bin_search(const int *a, int n, int key){
    int lo=0, hi=n-1; while(lo<=hi){ int m=(lo+hi)/2; if(a[m]==key) return m; if(a[m]<key) lo=m+1; else hi=m-1; } return -1; }

static int cmp_d(const void *x, const void *y){ double a=*(const double*)x, b=*(const double*)y; return (a>b)-(a<b); }
static void report(const char *sec, const char *label, long n, double *v, int k, long extra)
{
    qsort(v, k, sizeof(double), cmp_d);
    printf("DATA|%s|%s|%ld|%.3f|%.3f|%.3f|%ld\n", sec, label, n, v[k/2], v[0], v[k-1], extra);
}
#define RUNS 5
#define TIMED(sec,label,n,reps,extra,BODY) do { double v[RUNS]; long R=(reps); \
    for(int run=0;run<RUNS;run++){ clock_t t0=clock(); for(long r=0;r<R;r++){ BODY; } clock_t t1=clock(); \
      v[run]=(double)(t1-t0)/CLOCKS_PER_SEC*1e9/(double)R; } report(sec,label,n,v,RUNS,extra); } while(0)

static int vid(int k){ return 1 + 17 * k; }

int main(void)
{
    long ns[] = {200, 2000, 20000, 80000, 800000};
    for (int ni = 0; ni < 5; ni++) {
        int n = (int)ns[ni];
        long lin_reps = 60000000L / n; if (lin_reps < 20) lin_reps = 20;
        long fast_reps = 3000000L;
        Parking P; pk_init(&P, n);
        /* ---- build cost: n parks (hash insert + stack pop) ---- */
        { double v[RUNS];
          for (int run = 0; run < RUNS; run++) {
              Parking Q; clock_t t0 = clock(); pk_init(&Q, n); for (int k = 0; k < n; k++) pk_park(&Q, vid(k));
              clock_t t1 = clock(); v[run] = (double)(t1 - t0) / CLOCKS_PER_SEC * 1e9 / n; pk_destroy(&Q); }
          report("build", "init+n_parks_per_element", n, v, RUNS, 0); }
        for (int k = 0; k < n; k++) pk_park(&P, vid(k));          /* lot full; slot_vehicle sorted ascending */
        volatile int *va = (volatile int *)P.slot_vehicle;
        /* ---- count free: scan vs maintained counter ---- */
        TIMED("count","scan_O(n)",n,lin_reps,n, { va[0] = va[0]; sink += scan_count_free(P.slot_vehicle, n); });
        TIMED("count","counter_O(1)",n,fast_reps,1, { sink += pk_free_count(&P); });
        /* ---- lookup by vehicle id ---- */
        int worst = vid(n - 1);
        TIMED("lookup","linear_worst",n,lin_reps,n, { va[0] = va[0]; sink += lin_search(P.slot_vehicle, n, worst); });
        { unsigned x = 2463534242u; int i; TIMED("lookup","binary_sorted",n,fast_reps,0, { x ^= x << 13; x ^= x >> 17; x ^= x << 5; i = (int)(x % (unsigned)n); sink += bin_search(P.slot_vehicle, n, vid(i)); }); }
        { unsigned x = 2463534242u; int i; TIMED("lookup","hash",n,fast_reps,0, { x ^= x << 13; x ^= x >> 17; x ^= x << 5; i = (int)(x % (unsigned)n); sink += pk_lookup(&P, vid(i)); }); }
        /* ---- finding a free slot / park+leave pair, only slot n-1 free ---- */
        pk_leave_slot(&P, n - 1);
        TIMED("park","scan_first_free_O(n)",n,lin_reps,n, { va[0] = va[0]; sink += scan_first_free(P.slot_vehicle, n); });
        TIMED("park","stack_park_leave_pair",n,fast_reps,1, { int s = pk_park(&P, 5); sink += s; pk_leave_slot(&P, s); });
        pk_destroy(&P);
    }
    /* ---- different orderings: keeping the array sorted (insertion with shifting), n = 20000 ---- */
    { int n = 20000; int *keys = (int *)malloc(n * sizeof(int));
      for (int mode = 0; mode < 3; mode++) {                         /* 0 ascending, 1 random, 2 descending */
          for (int i = 0; i < n; i++) keys[i] = (mode == 0) ? i : (mode == 2) ? n - i : i;
          if (mode == 1) { unsigned s = 12345; for (int i = n - 1; i > 0; i--) { s = s * 1103515245u + 12345u; int j = (s >> 8) % (i + 1); int t = keys[i]; keys[i] = keys[j]; keys[j] = t; } }
          double v[3]; long shifts = 0;
          for (int run = 0; run < 3; run++) {
              SlotList L; sl_init(&L, n); shifts = 0; clock_t t0 = clock();
              for (int i = 0; i < n; i++) {
                  int lo = 0, hi = L.size;                           /* binary search for the insert position */
                  while (lo < hi) { int m = (lo + hi) / 2; if (L.data[m] < keys[i]) lo = m + 1; else hi = m; }
                  shifts += L.size - lo; sl_insert(&L, lo, keys[i]);
              }
              clock_t t1 = clock(); v[run] = (double)(t1 - t0) / CLOCKS_PER_SEC * 1e3; sink += L.data[0]; sl_free(&L);
          }
          qsort(v, 3, sizeof(double), cmp_d);
          printf("DATA|order|%s|%d|%.3f|%.3f|%.3f|%ld\n", mode == 0 ? "ascending" : mode == 1 ? "random" : "descending", n, v[1], v[0], v[2], shifts);
      } free(keys); }
    /* ---- growth policy: n = 20000 appends, grow-by-one vs doubling ---- */
    { int n = 20000; double v1[3], v2[3]; long c1 = 0, c2 = 0;
      for (int run = 0; run < 3; run++) {
          int *a = NULL; int cap = 0; c1 = 0; clock_t t0 = clock();
          for (int i = 0; i < n; i++) { if (i == cap) { int *b = (int *)malloc((size_t)(cap + 1) * sizeof(int)); for (int j = 0; j < cap; j++) { b[j] = a[j]; c1++; } free(a); a = b; cap++; } a[i] = i; }
          v1[run] = (double)(clock() - t0) / CLOCKS_PER_SEC * 1e3; sink += a[0]; free(a);
          a = NULL; cap = 0; c2 = 0; t0 = clock();
          for (int i = 0; i < n; i++) { if (i == cap) { int nc = cap ? cap * 2 : 1; int *b = (int *)malloc((size_t)nc * sizeof(int)); for (int j = 0; j < cap; j++) { b[j] = a[j]; c2++; } free(a); a = b; cap = nc; } a[i] = i; }
          v2[run] = (double)(clock() - t0) / CLOCKS_PER_SEC * 1e3; sink += a[0]; free(a);
      }
      qsort(v1, 3, sizeof(double), cmp_d); qsort(v2, 3, sizeof(double), cmp_d);
      printf("DATA|grow|by_one|%d|%.3f|%.3f|%.3f|%ld\n", n, v1[1], v1[0], v1[2], c1);
      printf("DATA|grow|doubling|%d|%.3f|%.3f|%.3f|%ld\n", n, v2[1], v2[0], v2[2], c2); }
    /* ---- hash statistics, 80,000 random keys, 106,667 buckets ---- */
    { HashMap m; int nb = 106667; hm_init(&m, nb); unsigned s = 2024; int ins = 0;
      while (ins < 80000) { s = s * 1103515245u + 12345u; int key = (int)((s >> 4) % 90000000u) + 1; if (hm_put(&m, key, ins) == 0) ins++; }
      int empty = 0, nonempty = 0; long tot = 0;
      for (int i = 0; i < nb; i++) { int c = 0; for (HNode *p = m.b[i]; p; p = p->next) c++; if (c == 0) empty++; else { nonempty++; tot += c; } }
      printf("DATA|hashstat|random_keys|%d|%.4f|%.4f|%.4f|%d\n", nb, (double)ins / nb, (double)tot / nonempty, (double)empty / nb, hm_maxchain(&m));
      hm_free(&m); }
    /* ---- Task 12: nearest free slot, scan (v1 style) vs heap (v2), lot half full (first n/2 slots occupied) ---- */
    for (int ni = 1; ni < 5; ni++) {
        int n = (int)ns[ni]; long lin_reps = 60000000L / n; if (lin_reps < 20) lin_reps = 20;
        int *occ = (int *)calloc(n, sizeof(int)); for (int i = 0; i < n / 2; i++) occ[i] = 1;
        volatile int *vo = (volatile int *)occ;
        TIMED("nearest","scan_nearest_O(n)",n,lin_reps,n/2, { vo[0] = vo[0]; sink += scan_first_free(occ, n); });
        Parking2 Q; pk2_init(&Q, n, 0, 0);
        for (int i = 0; i < n / 2; i++) pk2_park(&Q, vid(i), 0);        /* heap now holds slots n/2..n-1 */
        { int nxt = n; TIMED("nearest","heap_park_leave_pair",n,3000000L,0, { int s = pk2_park(&Q, nxt, 0); sink += s; pk2_leave_slot(&Q, s); }); }
        pk2_destroy(&Q); free(occ);
    }
    return 0;
}
