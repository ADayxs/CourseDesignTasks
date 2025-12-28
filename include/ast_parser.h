#pragma once

#ifndef COURSEDESIGNTASKS_AST_PARSER_H
#define COURSEDESIGNTASKS_AST_PARSER_H

#include <stddef.h>
#include "std_token.h"
#include "ast.h"

// 传入 Token* 数组和长度，返回 AST 根节点（用 ast_free 释放）
ASTNode* ast_parse_tokens(Token* const* toks, size_t ntoks);

#endif //COURSEDESIGNTASKS_AST_PARSER_H