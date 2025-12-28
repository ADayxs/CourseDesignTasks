#include <stdio.h>
#include <stddef.h>

#include "../include/ast_parser.h"
#include "../include/ast_serial.h"
#include "../include/ast.h"
#include "../include/std_token.h"

int main(void) {
    static char S_int[]    = "int";
    static char S_main[]   = "main";
    static char S_lpar[]   = "(";
    static char S_rpar[]   = ")";
    static char S_lbra[]   = "{";
    static char S_rbra[]   = "}";
    static char S_if[]     = "if";
    static char S_x[]      = "x";
    static char S_return[] = "return";
    static char S_0[]      = "0";
    static char S_1[]      = "1";
    static char S_semi[]   = ";";
    static char S_else[]   = "else";
    static char S_eof[]    = "<EOF>";

    // demo: int main(){ if(x) return 0; else return 1; }
    static Token toks_raw[] = {
        {TK_KEYWORD,     KW_INT,     S_int,    S_int,    1, 1},
        {TK_IDENT,       KW_UNKNOWN, S_main,   S_main,   1, 5},
        {TK_PUNCTUATION, KW_UNKNOWN, S_lpar,   S_lpar,   1, 9},
        {TK_PUNCTUATION, KW_UNKNOWN, S_rpar,   S_rpar,   1,10},
        {TK_PUNCTUATION, KW_UNKNOWN, S_lbra,   S_lbra,   1,12},

        {TK_KEYWORD,     KW_IF,      S_if,     S_if,     2, 3},
        {TK_PUNCTUATION, KW_UNKNOWN, S_lpar,   S_lpar,   2, 5},
        {TK_IDENT,       KW_UNKNOWN, S_x,      S_x,      2, 6},
        {TK_PUNCTUATION, KW_UNKNOWN, S_rpar,   S_rpar,   2, 7},

        {TK_KEYWORD,     KW_RETURN,  S_return, S_return, 2, 9},
        {TK_NUMBER,      KW_UNKNOWN, S_0,      S_0,      2,16},
        {TK_PUNCTUATION, KW_UNKNOWN, S_semi,   S_semi,   2,17},

        {TK_KEYWORD,     KW_ELSE,    S_else,   S_else,   3, 3},
        {TK_KEYWORD,     KW_RETURN,  S_return, S_return, 3, 8},
        {TK_NUMBER,      KW_UNKNOWN, S_1,      S_1,      3,15},
        {TK_PUNCTUATION, KW_UNKNOWN, S_semi,   S_semi,   3,16},

        {TK_PUNCTUATION, KW_UNKNOWN, S_rbra,   S_rbra,   4, 1},
        {TOKEN_EOF,      KW_UNKNOWN, S_eof,    S_eof,    4, 2},
    };

    enum { NTOKS = (int)(sizeof(toks_raw) / sizeof(toks_raw[0])) };
    static Token* toks[NTOKS];
    for (int i = 0; i < NTOKS; ++i) toks[i] = &toks_raw[i];

    // 1) 构建 AST
    ASTNode* root = ast_parse_tokens((Token* const*)toks, (size_t)NTOKS);
    if (!root) {
        fprintf(stderr, "[ERR] ast_parse_tokens failed\n");
        return 1;
    }

    // 2) 打印 AST
    puts("========== AST DUMP ==========");
    ast_dump(root, 0);

    // 3) 序列化打印
    puts("\n====== AST SERIAL (preorder) ======");
    StrVec sv;
    sv_init(&sv);

    if (!ast_serialize_preorder(root, &sv)) {
        fprintf(stderr, "[ERR] ast_serialize_preorder failed\n");
        ast_free(root);
        sv_free(&sv);
        return 1;
    }

    for (size_t i = 0; i < sv.size; ++i) {
        printf("%s%s", sv.data[i], (i + 1 == sv.size) ? "\n" : " ");
    }

    sv_free(&sv);
    ast_free(root);
    return 0;
}
