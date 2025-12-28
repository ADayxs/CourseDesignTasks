#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* ---------- read file ---------- */
char *read_file_all(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) { perror(path); return NULL; }
    if (fseek(f,0,SEEK_END)!=0) { fclose(f); return NULL; }
    long sz = ftell(f);
    fseek(f,0,SEEK_SET);
    char *buf = malloc(sz+1);
    if (!buf) { fclose(f); return NULL; }
    size_t r = fread(buf,1,sz,f);
    buf[r] = 0;
    fclose(f);
    return buf;
}

/* ---------- preprocess: remove // and comments, collapse whitespace ---------- */
char *preprocess_code(const char *src) {
    if (!src) return NULL;
    const char *p = src;
    char *out = malloc(strlen(src)+1);
    char *q = out;
    while (*p) {
        if (p[0]=='/' && p[1]=='/') {
            p += 2;
            while (*p && *p!='\n') p++;
        } else if (p[0]=='/' && p[1]=='*') {
            p += 2;
            while (*p && !(p[0]=='*' && p[1]=='/')) p++;
            if (*p) p += 2;
        } else {
            if (isspace((unsigned char)*p)) {
                // collapse whitespace to single space
                *q++ = ' ';
                while (isspace((unsigned char)*p)) p++;
            } else {
                *q++ = *p++;
            }
        }
    }
    *q = 0;
    // trim leading/trailing spaces
    char *s = out;
    while (*s && isspace((unsigned char)*s)) s++;
    char *end = out + strlen(out) - 1;
    while (end >= out && isspace((unsigned char)*end)) *end-- = 0;
    char *res = strdup(s);
    free(out);
    return res;
}

/* ---------- simple tokenizer ---------- */
/* tokens: identifiers, numbers, and single/multi-char punctuators/operators */
char **tokenize(const char *code, int *tokc, int normalize_ids) {
    int cap = 1024, n = 0;
    char **arr = malloc(sizeof(char*)*cap);
    const char *p = code;
    while (*p) {
        if (isspace((unsigned char)*p)) { p++; continue; }
        if (isalpha((unsigned char)*p) || *p=='_') {
            const char *s = p;
            while (isalnum((unsigned char)*p) || *p=='_') p++;
            int L = p - s;
            char *t = malloc(L+1);
            memcpy(t, s, L); t[L]=0;
            if (normalize_ids && strcmp(t,"main")!=0) { free(t); t = strdup("id"); }
            if (n==cap) { cap*=2; arr = realloc(arr, sizeof(char*)*cap); }
            arr[n++] = t;
        } else if (isdigit((unsigned char)*p)) {
            const char *s = p;
            while (isdigit((unsigned char)*p)) p++;
            int L = p - s;
            char *t = malloc(L+1);
            memcpy(t, s, L); t[L]=0;
            if (n==cap) { cap*=2; arr = realloc(arr, sizeof(char*)*cap); }
            arr[n++] = t;
        } else {
            // two-char ops
            if ((p[0]=='=' && p[1]=='=') || (p[0]=='+'&&p[1]=='+') || (p[0]=='-'&&p[1]=='-')
                || (p[0]=='!'&&p[1]=='=') || (p[0]=='<'&&p[1]=='=') || (p[0]=='>'&&p[1]=='=')
                || (p[0]=='&'&&p[1]=='&') || (p[0]=='|'&&p[1]=='|')) {
                char tmp[3] = {p[0], p[1], 0};
                if (n==cap) { cap*=2; arr = realloc(arr, sizeof(char*)*cap); }
                arr[n++] = strdup(tmp);
                p += 2;
            } else {
                char tmp[2] = {p[0], 0};
                if (n==cap) { cap*=2; arr = realloc(arr, sizeof(char*)*cap); }
                arr[n++] = strdup(tmp);
                p++;
            }
        }
    }
    *tokc = n;
    return arr;
}
void free_tokens(char **toks, int n) { for (int i=0;i<n;i++) free(toks[i]); free(toks); }

/* ---------- simple vocab builder (linear search, fine for two docs) ---------- */
typedef struct {
    char **terms; int n; int cap;
} Vocab;
void vocab_init(Vocab *v){ v->n=0; v->cap=256; v->terms=malloc(sizeof(char*)*v->cap); }
int vocab_index_of(Vocab *v, const char *t){
    for (int i=0;i<v->n;i++) if (strcmp(v->terms[i], t)==0) return i;
    return -1;
}
int vocab_add(Vocab *v, const char *t){
    int idx = vocab_index_of(v,t);
    if (idx>=0) return idx;
    if (v->n==v->cap){ v->cap*=2; v->terms = realloc(v->terms, sizeof(char*)*v->cap); }
    v->terms[v->n] = strdup(t);
    return v->n++;
}
void vocab_free(Vocab *v){ for (int i=0;i<v->n;i++) free(v->terms[i]); free(v->terms); v->n=0; }

/* ---------- Levenshtein (two-row) ---------- */
int levenshtein(const char *s, const char *t) {
    int n = strlen(s), m = strlen(t);
    if (n==0) return m;
    if (m==0) return n;
    int *v0 = malloc((m+1)*sizeof(int));
    int *v1 = malloc((m+1)*sizeof(int));
    for (int j=0;j<=m;j++) v0[j]=j;
    for (int i=1;i<=n;i++){
        v1[0]=i;
        for (int j=1;j<=m;j++){
            int cost = (s[i-1]==t[j-1])?0:1;
            int del = v0[j]+1;
            int ins = v1[j-1]+1;
            int sub = v0[j-1]+cost;
            int mn = del<ins?del:ins; mn = mn<sub?mn:sub;
            v1[j]=mn;
        }
        memcpy(v0, v1, (m+1)*sizeof(int));
    }
    int res = v0[m];
    free(v0); free(v1);
    return res;
}

/* ---------- LCS length (DP, O(min(n,m)) space) ---------- */
int lcs_length(const char *a, const char *b) {
    int n=strlen(a), m=strlen(b);
    if (n==0 || m==0) return 0;
    int *dp = calloc(m+1, sizeof(int));
    for (int i=1;i<=n;i++){
        int prev = 0;
        for (int j=1;j<=m;j++){
            int tmp = dp[j];
            if (a[i-1]==b[j-1]) dp[j] = prev + 1;
            else dp[j] = dp[j] > dp[j-1] ? dp[j] : dp[j-1];
            prev = tmp;
        }
    }
    int res = dp[m]; free(dp); return res;
}

/* ---------- compute sparse cosine between two (idx[], val[], len) ---------- */
double sparse_cosine(int *idxA, double *valA, int na, int *idxB, double *valB, int nb) {
    double dot = 0.0;
    // choose smaller to iterate
    if (na <= nb) {
        for (int i=0;i<na;i++){
            int id = idxA[i];
            double va = valA[i];
            for (int j=0;j<nb;j++) if (idxB[j]==id) { dot += va * valB[j]; break; }
        }
    } else {
        for (int j=0;j<nb;j++){
            int id = idxB[j];
            double vb = valB[j];
            for (int i=0;i<na;i++) if (idxA[i]==id) { dot += vb * valA[i]; break; }
        }
    }
    return dot; // note: caller should ensure vectors are normalized
}

/* ---------------- main diagnostic routine ---------------- */
int main(int argc, char **argv){
    if (argc < 3) {
        fprintf(stderr,"Usage: %s code1.c code2.c\n", argv[0]);
        return 1;
    }
    const char *fA = argv[1], *fB = argv[2];
    char *rawA = read_file_all(fA);
    char *rawB = read_file_all(fB);
    if (!rawA || !rawB) return 1;

    char *preA = preprocess_code(rawA);
    char *preB = preprocess_code(rawB);

    int tokA, tokB;
    // keep identifiers (normalize_ids = 0)
    char **toksA = tokenize(preA, &tokA, 0);
    char **toksB = tokenize(preB, &tokB, 0);

    printf("Tokens: A=%d, B=%d\n", tokA, tokB);

    // build vocab
    Vocab vocab; vocab_init(&vocab);
    // collect unique terms from both docs
    for (int i=0;i<tokA;i++) vocab_add(&vocab, toksA[i]);
    for (int i=0;i<tokB;i++) vocab_add(&vocab, toksB[i]);
    int V = vocab.n;
    printf("Vocab size (union): %d\n", V);

    // compute tf per doc (dense arrays size V)
    int *tfA = calloc(V, sizeof(int));
    int *tfB = calloc(V, sizeof(int));
    int *df = calloc(V, sizeof(int));
    for (int i=0;i<tokA;i++){
        int id = vocab_index_of(&vocab, toksA[i]);
        if (id>=0) tfA[id] += 1;
    }
    for (int i=0;i<tokB;i++){
        int id = vocab_index_of(&vocab, toksB[i]);
        if (id>=0) tfB[id] += 1;
    }
    for (int i=0;i<V;i++){
        if (tfA[i] > 0) df[i] ++;
        if (tfB[i] > 0) df[i] ++;
    }

    // print some df info
    int nonzero_terms = 0;
    for (int i=0;i<V;i++) if (df[i]>0) nonzero_terms++;
    printf("Terms appearing in at least one doc: %d\n", nonzero_terms);

    // smoothed idf
    double *idf = malloc(sizeof(double)*V);
    for (int i=0;i<V;i++){
        idf[i] = log(1.0 + 2.0 / (1.0 + (double)df[i])); // N=2
    }

    // debug: show top terms by idf (largest)
    printf("\nTop terms (term : df, idf) sample:\n");
    int shown=0;
    for (int i=0;i<V && shown<40;i++){
        if (df[i]==0) continue;
        printf("  \"%s\" : df=%d idf=%.4f\n", vocab.terms[i], df[i], idf[i]);
        shown++;
    }

    // build sparse TF-IDF vectors (using log-tf)
    int *idxA = malloc(sizeof(int)*V); double *valA = malloc(sizeof(double)*V); int nA=0;
    int *idxB = malloc(sizeof(int)*V); double *valB = malloc(sizeof(double)*V); int nB=0;
    double normA2 = 0.0, normB2 = 0.0;
    for (int i=0;i<V;i++){
        if (tfA[i]>0) {
            double tfw = 1.0 + log((double)tfA[i]);
            double w = tfw * idf[i];
            if (w != 0.0) { idxA[nA]=i; valA[nA]=w; normA2 += w*w; nA++; }
        }
        if (tfB[i]>0) {
            double tfw = 1.0 + log((double)tfB[i]);
            double w = tfw * idf[i];
            if (w != 0.0) { idxB[nB]=i; valB[nB]=w; normB2 += w*w; nB++; }
        }
    }
    double normA = (normA2>0)? sqrt(normA2) : 1.0;
    double normB = (normB2>0)? sqrt(normB2) : 1.0;
    // normalize
    for (int i=0;i<nA;i++) valA[i] /= normA;
    for (int i=0;i<nB;i++) valB[i] /= normB;

    printf("\nSparse vector stats: A nonzero=%d, B nonzero=%d\n", nA, nB);

    // compute cosine
    double dot = sparse_cosine(idxA,valA,nA, idxB,valB,nB);
    double cosine = dot; // already normalized
    printf("TF-IDF cosine similarity = %.6f\n", cosine);

    // if both vectors empty, tell user
    if (nA==0 || nB==0) {
        printf("WARNING: one of the TF-IDF vectors is empty (all weights zero).\n");
        printf("This typically happens when most tokens appear in both documents (df=2)\n");
        printf("and an unsmoothed idf was used. This program uses smoothed idf to avoid that.\n");
    }

    // Levenshtein normalized
    int lev = levenshtein(preA, preB);
    int maxlen = (int) (strlen(preA) > strlen(preB) ? strlen(preA) : strlen(preB));
    double lev_sim = 1.0 - ((maxlen>0)? (double)lev / (double)maxlen : 0.0);
    printf("Levenshtein distance = %d, normalized similarity = %.6f\n", lev, lev_sim);

    // LCS
    int lcs = lcs_length(preA, preB);
    double lcs_ratio = (double)lcs / (double)( (strlen(preA)>strlen(preB))?strlen(preA):strlen(preB) );
    printf("LCS length = %d, ratio = %.6f\n", lcs, lcs_ratio);

    // quick token Jaccard
    int inter = 0;
    for (int i=0;i<tokA;i++){
        for (int j=0;j<tokB;j++) if (strcmp(toksA[i], toksB[j])==0) { inter++; break; }
    }
    double jacc = (double)inter / (double)(tokA + tokB - inter);
    printf("Token Jaccard = %.6f (intersection=%d)\n", jacc, inter);

    // cleanup
    free(rawA); free(rawB);
    free(preA); free(preB);
    free_tokens(toksA, tokA); free_tokens(toksB, tokB);
    vocab_free(&vocab);
    free(tfA); free(tfB); free(df); free(idf);
    free(idxA); free(idxB); free(valA); free(valB);

    return 0;
}