/* q6_predict.c - Task 6: predict-the-output program for a trie */
#include <stdio.h>
#include "trie.h"

int main(void)
{
    const char *words[] = {"to", "tea", "ted", "ten", "a", "inn"};
    Trie t;
    trie_init(&t);
    for (int i = 0; i < 6; i++) trie_insert(&t, words[i]);
    printf("nodes = %zu, words = %zu\n", t.nodes, t.words);
    printf("search(ten) = %d, search(te) = %d\n", trie_search(&t, "ten"), trie_search(&t, "te"));
    printf("startsWith(te) = %d, startsWith(tex) = %d\n", trie_starts_with(&t, "te"), trie_starts_with(&t, "tex"));
    printf("prefix counts: t = %d, te = %d, i = %d, z = %d, empty = %d\n", trie_prefix_count(&t, "t"),
           trie_prefix_count(&t, "te"), trie_prefix_count(&t, "i"), trie_prefix_count(&t, "z"),
           trie_prefix_count(&t, ""));
    char out[10][TRIE_MAXW];
    int n = trie_autocomplete(&t, "te", 10, out);
    printf("autocomplete(te):");
    for (int i = 0; i < n; i++) printf(" %s", out[i]);
    printf("\n");
    int d1 = trie_delete(&t, "ted");
    printf("delete(ted) = %d: nodes = %zu, words = %zu, count(te) = %d\n", d1, t.nodes, t.words, trie_prefix_count(&t, "te"));
    printf("delete(te) = %d (not a stored word)\n", trie_delete(&t, "te"));
    printf("memory = %zu nodes * %zu bytes = %zu bytes\n", t.nodes, sizeof(TNode), trie_bytes(&t));
    trie_free(&t);
    return 0;
}
