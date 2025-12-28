#include "../include//edit_distance.c.h"
#include <stdlib.h>
#include <string.h>

static inline size_t min3(const size_t a, const size_t b, const size_t c) {
    return a < b ? a : b < c ? b : c;
}

static inline int eq(const char *x, const char *y) {
    if (x == y) return 1;
    if (!x || !y) return 0;
    return strcmp(x, y) == 0;
}

size_t levenshtein_strvec(const StrVec *a, const StrVec *b) {
    if (!a || !b) return 0;

    const StrVec *A = a, *B = b;
    size_t n = a->size, m = b->size;
    if (m > n) { A = b; B = a; n = A->size; m = B->size; }
    if (m == 0) return n;
    if (n == 0) return m;

    size_t *v[2];
    v[0] = (size_t*)malloc((m + 1) * sizeof(size_t));
    v[1] = (size_t*)malloc((m + 1) * sizeof(size_t));
    if (!v[0] || !v[1]) { free(v[0]); free(v[1]); return 0; }

    for (size_t j = 0; j <= m; ++ j) {
        v[0][j] = j;
    }

    int cur = 1;  // 当前行索引
    int prev = 0; // 上一行索引

    for (size_t i = 1; i <= n; ++ i) {
        v[cur][0] = i;
        const char *ai = A->data[i - 1];

        for (size_t j = 1; j <= m; ++ j) {
            const char *bi = B->data[j - 1];
            const size_t cost = eq(ai, bi) ? 0 : 1;

            const size_t del = v[prev][j] + 1;
            const size_t ins = v[cur][j - 1] + 1;
            const size_t sub = v[prev][j - 1] + cost;

            v[cur][j] = min3(del, ins, sub);
        }

        cur ^= 1;
        prev ^= 1;
    }

    const size_t dist = v[prev][m];
    free(v[0]);
    free(v[1]);

    return dist;
}

double similarity_from_dist(const size_t dist, const size_t lenA, const size_t lenB) {
    const size_t mx = (lenA > lenB) ? lenA : lenB;
    if (mx == 0) return 1.0;
    return 1.0 - (double)dist / (double)mx;
}