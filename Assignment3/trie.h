/* trie.h - Task 6: trie over the lowercase letters a-z. Each node holds 26
   child pointers, an end-of-word flag and cnt, the number of stored words in
   the subtree rooted at the node (used for O(L) prefix counting). One node is
   216 bytes: 26 * 8 + 4 + 4. steps counts the child links followed. */
#ifndef TRIE_H
#define TRIE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRIE_MAXW 32

typedef struct TNode {
    struct TNode *c[26];
    int end;
    int cnt;
} TNode;

typedef struct {
    TNode *root;
    size_t nodes, words;
    long steps;
} Trie;

static inline TNode *trie_new_node(Trie *t)
{
    TNode *n = calloc(1, sizeof *n);
    if (!n) exit(1);
    t->nodes++;
    return n;
}

static inline void trie_init(Trie *t)
{
    t->nodes = 0; t->words = 0; t->steps = 0;
    t->root = trie_new_node(t);
}

static inline int trie_valid_word(const char *w)
{
    if (!*w) return 0;
    for (; *w; w++) if (*w < 'a' || *w > 'z') return 0;
    return 1;
}

/* node reached by following w, or NULL; the empty string gives the root */
static inline TNode *trie_find(Trie *t, const char *w)
{
    TNode *n = t->root;
    for (; *w; w++) {
        if (*w < 'a' || *w > 'z') return NULL;
        n = n->c[*w - 'a'];
        t->steps++;
        if (!n) return NULL;
    }
    return n;
}

static inline int trie_search(Trie *t, const char *w)
{
    TNode *n = trie_find(t, w);
    return n && n->end;
}

static inline int trie_starts_with(Trie *t, const char *prefix) { return trie_find(t, prefix) != NULL; }

static inline int trie_prefix_count(Trie *t, const char *prefix)
{
    TNode *n = trie_find(t, prefix);
    return n ? n->cnt : 0;
}

/* returns 1 when inserted, 0 when already stored, -1 for an invalid word */
static inline int trie_insert(Trie *t, const char *w)
{
    if (!trie_valid_word(w)) return -1;
    long saved = t->steps;
    int exists = trie_search(t, w);
    t->steps = saved;
    if (exists) return 0;
    TNode *n = t->root;
    n->cnt++;
    for (; *w; w++) {
        int i = *w - 'a';
        if (!n->c[i]) n->c[i] = trie_new_node(t);
        n = n->c[i];
        t->steps++;
        n->cnt++;
    }
    n->end = 1;
    t->words++;
    return 1;
}

static inline void trie_del_rec(Trie *t, TNode *n, const char *w)
{
    n->cnt--;
    if (!*w) { n->end = 0; return; }
    int i = *w - 'a';
    TNode *ch = n->c[i];
    trie_del_rec(t, ch, w + 1);
    if (ch->cnt == 0) {                    /* nothing left below: prune the branch */
        free(ch);
        n->c[i] = NULL;
        t->nodes--;
    }
}

/* returns 1 when removed, 0 when the word is not stored */
static inline int trie_delete(Trie *t, const char *w)
{
    long saved = t->steps;
    int exists = trie_valid_word(w) && trie_search(t, w);
    t->steps = saved;
    if (!exists) return 0;
    trie_del_rec(t, t->root, w);
    t->words--;
    return 1;
}

static inline void trie_collect(const TNode *n, char *buf, int len, int limit, char (*out)[TRIE_MAXW], int *found)
{
    if (*found >= limit) return;
    if (n->end) {
        buf[len] = '\0';
        strcpy(out[(*found)++], buf);
    }
    for (int i = 0; i < 26 && *found < limit; i++)
        if (n->c[i]) {
            buf[len] = (char)('a' + i);
            trie_collect(n->c[i], buf, len + 1, limit, out, found);
        }
}

/* words that start with prefix in alphabetical order, at most limit of them */
static inline int trie_autocomplete(Trie *t, const char *prefix, int limit, char (*out)[TRIE_MAXW])
{
    TNode *n = trie_find(t, prefix);
    if (!n) return 0;
    char buf[TRIE_MAXW];
    size_t len = strlen(prefix);
    if (len >= TRIE_MAXW - 1) return 0;
    memcpy(buf, prefix, len);
    int found = 0;
    trie_collect(n, buf, (int)len, limit, out, &found);
    return found;
}

static inline size_t trie_bytes(const Trie *t) { return t->nodes * sizeof(TNode); }

static inline void trie_free_node(TNode *n)
{
    if (!n) return;
    for (int i = 0; i < 26; i++) trie_free_node(n->c[i]);
    free(n);
}

static inline void trie_free(Trie *t) { trie_free_node(t->root); t->root = NULL; t->nodes = 0; t->words = 0; }
#endif
