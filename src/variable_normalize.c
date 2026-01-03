#include "../include/variable_normalise.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char* xstrdup(const char* s)
{
    if (!s) return NULL;
    size_t n = strlen(s);
    char* p = (char*)malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, s, n + 1);
    return p;
}

void vn_init(VarNormalizeContext* ctx)
{
    if (!ctx) return;
    
    ctx->var_names = NULL;
    ctx->var_count = 0;
    ctx->var_cap = 0;
    ctx->next_var_id = 1;
}

const char* vn_normalize_identifier(VarNormalizeContext* ctx, const char* original_name)
{
    if (!ctx || !original_name) return "var0";
    
    // 检查是否已经归一化过这个变量�??
    for (size_t i = 0; i < ctx->var_count; i++) {
        if (strcmp(ctx->var_names[i], original_name) == 0) {
            // 生成归一化名称：var + 索引
            static char normalized[32];
            snprintf(normalized, sizeof(normalized), "var%zu", i + 1);
            return normalized;
        }
    }
    
    // 新变量名，添加到上下�??
    if (ctx->var_count >= ctx->var_cap) {
        size_t new_cap = (ctx->var_cap == 0) ? 16 : ctx->var_cap * 2;
        char** new_names = (char**)realloc(ctx->var_names, new_cap * sizeof(char*));
        if (!new_names) return "var0";
        
        ctx->var_names = new_names;
        ctx->var_cap = new_cap;
    }
    
    // 存储原始变量�??
    ctx->var_names[ctx->var_count] = xstrdup(original_name);
    if (!ctx->var_names[ctx->var_count]) return "var0";
    
    // 生成归一化名�??
    static char normalized[32];
    snprintf(normalized, sizeof(normalized), "var%zu", ctx->var_count + 1);
    ctx->var_count++;
    
    return normalized;
}

void vn_free(VarNormalizeContext* ctx)
{
    if (!ctx) return;
    
    for (size_t i = 0; i < ctx->var_count; i++) {
        free(ctx->var_names[i]);
    }
    free(ctx->var_names);
    
    ctx->var_names = NULL;
    ctx->var_count = 0;
    ctx->var_cap = 0;
    ctx->next_var_id = 1;
}

void normalize_variable_names(Token** tokens, size_t token_count, VarNormalizeContext* ctx)
{
    if (!tokens || !ctx) return;
    
    for (size_t i = 0; i < token_count; i++) {
        Token* token = tokens[i];
        if (!token) continue;
        
        // 只对标识符类型的token进行归一�??
        if (token->type == TK_IDENT) {
            // 检查是否是C语言关键字，避免对关键字进行归一�??
            const char* identifiers_to_preserve[] = {
                "if", "else", "for", "while", "do", "switch", "case", "default",
                "return", "break", "continue", "int", "char", "float", "double",
                "void", "struct", "typedef", "sizeof", "include", "define",
                NULL
            };
            
            int is_keyword = 0;
            for (const char** kw = identifiers_to_preserve; *kw; kw++) {
                if (token->lex && strcmp(token->lex, *kw) == 0) {
                    is_keyword = 1;
                    break;
                }
            }
            
            // 对非关键字的标识符进行归一�??
            if (!is_keyword && token->lex) {
                const char* normalized = vn_normalize_identifier(ctx, token->lex);
                if (normalized) {
                    // 更新token的lex字段为归一化后的名�??
                    free(token->lex);
                    token->lex = xstrdup(normalized);
                }
            }
        }
    }
}