#pragma once

#ifndef COURSEDESIGNTASKS_AST_H
#define COURSEDESIGNTASKS_AST_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    AST_PROGRAM = 0,
    AST_FUNCTION,
    AST_BLOCK,

    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_DO_WHILE,
    AST_SWITCH,
    AST_CASE,
    AST_DEFAULT,

    AST_RETURN,
    AST_BREAK,
    AST_CONTINUE,

    AST_STMT,   // 普通语句
    AST_EXPR,   // 括号表达式 or case 表达式
    AST_TOKEN   // 叶子：保存一个 token 的“标签”
} ASTKind;

typedef struct ASTNode {
    ASTKind kind;
    char* text;
    struct ASTNode** children;
    size_t child_count;
    size_t child_cap;
} ASTNode;

ASTNode* ast_new(ASTKind kind, const char* text);
bool     ast_add_child(ASTNode* parent, ASTNode* child);
void     ast_free(ASTNode* node);

const char* ast_kind_name(ASTKind kind);

void ast_dump(const ASTNode* node, int indent);

#endif //COURSEDESIGNTASKS_AST_H