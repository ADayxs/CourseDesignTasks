#ifndef COURSEDESIGNTASKS_VARIABLE_NORMALIZE_H
#define COURSEDESIGNTASKS_VARIABLE_NORMALIZE_H

#include "std_token.h"
#include <stddef.h>

typedef struct
{
    char** var_names;
    size_t var_count;
    size_t var_cap;
    size_t next_var_id;
} VarNormalizeContext;

// 初始化归一化上下文
void vn_init(VarNormalizeContext* ctx);

// 归一化变量名：将原始变量名映射为统一格式（如 var1, var2, ...�?
// 返回归一化后的名称（不需要释放，由上下文管理�?
const char* vn_normalize_identifier(VarNormalizeContext* ctx, const char* original_name);

void vn_free(VarNormalizeContext* ctx);

void normalize_variable_names(Token** tokens, size_t token_count, VarNormalizeContext* ctx);

#endif // COURSEDESIGNTASKS_VARIABLE_NORMALIZE_H