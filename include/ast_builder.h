#ifndef AST_BUILDER_H
#define AST_BUILDER_H

#include "ast.h"
#include "tokenizer.h"
#include <vector>
#include <string>

// Build an AST from a sequence of tokens
ASTNode* buildAST(const std::vector<token::Token>& tokens);

// Get the root of the AST (implementation of function declared in parser.h)
ASTNode* getASTRoot();

#endif // AST_BUILDER_H
