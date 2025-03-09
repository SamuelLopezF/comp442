#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"  // This provides token::Token and token::Lexeme.
#include "ast_builder.h" // Add this line for AST generation
#include <map>
#include <string>
#include <vector>
using std::map;
using std::string;
using std::vector;

// Splits a production string (symbols separated by spaces) into a vector of symbols.
vector<string> splitProduction(const string &prod);

// Builds and returns the LL(1) parsing table as a nested map.
map<string, map<string, string>> buildParsingTable();

// Incremental parser interface:
//
// Call initParserState() once before feeding tokens.
// Then, feed one token at a time via feedToken().
// The parser uses a static parse stack and table.
void initParserState();

// Initializes with an AST builder
void initParserState(ASTBuilder* astBuilder);

// Feeds a single token to the parser. Returns true if the token was accepted;
// returns false if a syntax error occurred.
bool feedToken(const token::Token &token);

// Get the AST builder instance
ASTBuilder* getASTBuilder();

#endif // PARSER_H
