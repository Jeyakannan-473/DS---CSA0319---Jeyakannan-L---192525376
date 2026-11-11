/* q6_trie_demo.c - Task 6: trie operations, prefix tests, boundary tests and
   a randomised check against a brute-force model. The TODO items of the
   exercise skeleton are marked and completed in trie.h:
     TODO 1  trie_insert       create missing child nodes along the word
     TODO 2  trie_find         follow child links, stop on a missing link
     TODO 3  trie_del_rec      prune nodes whose subtree became empty
     TODO 4  trie_collect      depth-first walk in alphabetical order */
#include <stdio.h>
#include <string.h>
#include "trie.h"

static void list(Trie *t, const char *prefix, int limit)
{
    char out[20][TRIE_MAXW];
    int n = trie_autocomplete(t, prefix, limit, out);
    printf("autocomplete(\"%s\", limit %d) -> %d result(s):", prefix, limit, n);
    for (int i = 0; i < n; i++) printf(" %s", out[i]);
    printf("\n");
}

int main(void)
{
    Trie t;
    trie_init(&t);
    const char *words[] = {"apple", "app", "apply", "apt", "bat", "bath", "bad", "cat", "cattle", "car"};
    printf("PART A - insertion (nodes created per word)\n");
    for (int i = 0; i < 10; i++) {
        size_t before = t.nodes;
        int r = trie_insert(&t, words[i]);
        printf("insert %-7s -> %d, new nodes = %zu, total nodes = %zu\n", words[i], r, t.nodes - before, t.nodes);
    }
    printf("words = %zu, nodes = %zu, bytes = %zu\n", t.words, t.nodes, trie_bytes(&t));

    printf("\nPART B - search, prefix and count tests\n");
    const char *probe[] = {"app", "appl", "apple", "applesauce", "bath", "ba", "cattle", "dog", "a"};
    for (int i = 0; i < 9; i++) {
        t.steps = 0;
        int s = trie_search(&t, probe[i]);
        long st = t.steps;
        int p = trie_starts_with(&t, probe[i]);
        printf("%-11s search = %d, startsWith = %d, steps for search = %ld\n", probe[i], s, p, st);
    }
    const char *pre[] = {"ap", "app", "ba", "c", "cat", "z", ""};
    for (int i = 0; i < 7; i++)
        printf("count of words with prefix \"%s\" = %d\n", pre[i], trie_prefix_count(&t, pre[i]));

    printf("\nPART C - autocomplete\n");
    list(&t, "ap", 10);
    list(&t, "ba", 2);
    list(&t, "cat", 10);
    list(&t, "z", 10);
    list(&t, "", 4);

    printf("\nPART D - deletion\n");
    size_t n0 = t.nodes;
    int d = trie_delete(&t, "app");
    printf("delete app    -> %d, nodes %zu -> %zu (path kept: apple and apply live below), search(app) = %d, search(apple) = %d\n",
           d, n0, t.nodes, trie_search(&t, "app"), trie_search(&t, "apple"));
    n0 = t.nodes;
    int r = trie_delete(&t, "apt");
    printf("delete apt    -> %d, nodes %zu -> %zu (leaf pruned), startsWith(apt) = %d\n", r, n0, t.nodes, trie_starts_with(&t, "apt"));
    n0 = t.nodes;
    r = trie_delete(&t, "cattle");
    printf("delete cattle -> %d, nodes %zu -> %zu (tle pruned, cat stays), search(cat) = %d\n", r, n0, t.nodes, trie_search(&t, "cat"));
    r = trie_delete(&t, "ghost");
    printf("delete ghost  -> %d (absent word), nodes = %zu\n", r, t.nodes);
    r = trie_delete(&t, "ap");
    printf("delete ap     -> %d (a prefix but not a stored word)\n", r);

    printf("\nPART E - invalid input and duplicates\n");
    printf("insert \"\"      -> %d\n", trie_insert(&t, ""));
    printf("insert \"Cat\"   -> %d\n", trie_insert(&t, "Cat"));
    printf("insert \"c4t\"   -> %d\n", trie_insert(&t, "c4t"));
    printf("insert \"car\"   -> %d (duplicate)\n", trie_insert(&t, "car"));
    printf("search(\"CAR\") = %d, startsWith(\"Ca\") = %d\n", trie_search(&t, "CAR"), trie_starts_with(&t, "Ca"));
    trie_free(&t);

    printf("\nPART F - 20000 random operations against a brute-force model\n");
    /* universe: every word over {a,b,c} of length 1..4 (120 words) */
    static char uni[120][5];
    int un = 0;
    for (int len = 1; len <= 4; len++) {
        int total = 1;
        for (int i = 0; i < len; i++) total *= 3;
        for (int v = 0; v < total; v++) {
            int x = v;
            for (int i = len - 1; i >= 0; i--) { uni[un][i] = (char)('a' + x % 3); x /= 3; }
            uni[un][len] = '\0';
            un++;
        }
    }
    /* sort the universe alphabetically for the model */
    for (int i = 0; i < un; i++)
        for (int j = i + 1; j < un; j++)
            if (strcmp(uni[i], uni[j]) > 0) { char tmp[5]; strcpy(tmp, uni[i]); strcpy(uni[i], uni[j]); strcpy(uni[j], tmp); }
    static char present[120];
    memset(present, 0, sizeof present);
    trie_init(&t);
    unsigned long long s = 88172645463325252ULL;
    long mism = 0;
    for (int step = 1; step <= 20000; step++) {
        s ^= s << 13; s ^= s >> 7; s ^= s << 17;
        int w = (int)(s % 120), op = (int)((s >> 20) % 5);
        if (op == 0) { int r2 = trie_insert(&t, uni[w]); if (r2 != !present[w]) mism++; present[w] = 1; }
        else if (op == 1) { int r2 = trie_delete(&t, uni[w]); if (r2 != present[w]) mism++; present[w] = 0; }
        else if (op == 2) { if (trie_search(&t, uni[w]) != present[w]) mism++; }
        else if (op == 3) {
            int want = 0;
            for (int i = 0; i < un; i++) if (present[i] && strncmp(uni[i], uni[w], strlen(uni[w])) == 0) want++;
            if (trie_prefix_count(&t, uni[w]) != want) mism++;
        } else {
            char out[8][TRIE_MAXW];
            int got = trie_autocomplete(&t, uni[w], 8, out), k = 0;
            for (int i = 0; i < un && k < got; i++)
                if (present[i] && strncmp(uni[i], uni[w], strlen(uni[w])) == 0) { if (strcmp(out[k], uni[i]) != 0) mism++; k++; }
            int total = 0;
            for (int i = 0; i < un; i++) if (present[i] && strncmp(uni[i], uni[w], strlen(uni[w])) == 0) total++;
            if (got != (total < 8 ? total : 8)) mism++;
        }
    }
    int live = 0;
    for (int i = 0; i < un; i++) live += present[i];
    printf("final words = %zu (model %d), nodes = %zu, mismatches with the model = %ld\n", t.words, live, t.nodes, mism);
    trie_free(&t);
    return 0;
}
