#include "../include/ast_serial.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char *xstrdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s);
    char *p = (char *)malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, s, n + 1);
    return p;
}

void sv_init(StrVec* v) {
    v->data = NULL;
    v->size = 0;
    v->cap = 0;
}

bool sv_push(StrVec* v, const char* s) {
    if (!v || !s) return false;

    char* dup = xstrdup(s);
    if (!dup) return false;

    if (v->size == v->cap) {
        size_t nc = (v->cap == 0) ? 16 : v->cap * 2;
        char** p = (char**)realloc(v->data, nc * sizeof(char*));
        if (!p) {
            free(dup);
            return false;
        }
        v->data = p;
        v->cap = nc;
    }

    v->data[v->size++] = dup;
    return true;
}

void sv_free(StrVec* v) {
    if (!v) return;
    for (size_t i = 0; i < v->size; ++i) free(v->data[i]);
    free(v->data);
    v->data = NULL;
    v->size = 0;
    v->cap = 0;
}

static bool emit_node(const ASTNode* n, StrVec* out) {
    if (!n) return true;

    char buf[64];
    snprintf(buf, sizeof(buf), "<%s>", ast_kind_name(n->kind));
    if (!sv_push(out, buf)) return false;

    if (n->kind == AST_TOKEN && n->text) {
        if (!sv_push(out, n->text)) return false;
    }

    for (size_t i = 0; i < n->child_count; i ++) {
        if (!emit_node(n->children[i], out)) return false;
    }

    snprintf(buf, sizeof(buf), "</%s>", ast_kind_name(n->kind));
    if (!sv_push(out, buf)) return false;

    return true;
}

bool ast_serialize_preorder(const ASTNode* root, StrVec* out) {
    if (!root || !out) return false;
    return emit_node(root, out);
}