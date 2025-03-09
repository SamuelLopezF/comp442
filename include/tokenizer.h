#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>
#include <cstddef>
#include <regex>
#include <fstream>
#include <iostream>
using std::string;
using std::vector;
using std::size_t;

namespace token {

// A simple Token structure.
struct Token {
    string type;
    string value;
};

// Lexeme holds a token and the number of characters consumed.
struct Lexeme {
    Token token;
    size_t length;
};

// A structure to hold the state of the tokenizer.
struct State {
    int new_state;
    int letter_start;
    int non_zero_digit_start;
    int zero_start;
    int special_char_start;
    int single_char_operator_start;
    int single_line_comment_start;
    int multi_line_comment_start;
    // Layer two:
    int interger_start;
    int fraction_start;
    int two_char_operator_start;
};

class Tokenizer {
public:
    // Constructs a tokenizer for the given filename.
    Tokenizer(string filename);

    // Ingests characters starting at c.
    // Returns a Lexeme which contains the token (with type and value)
    // and the number of characters that were matched.
    Lexeme IngestChar(const char *c);

    // Tries to form a new token from the characters starting at c.
    // Returns a Lexeme containing the token and match length.
    Lexeme NewToken(const char *c);

    // Sets up the output files.
    void setupOutputFile();

    // Writes a token (token type and token value) to the tokens file.
    void writeTokens(const string &tokenType, const string &tokenValue);

    // Writes an error message to the errors file.
    void writeErrors(const string &error);

private:
    string filename;
    string outputErrorsFileName;
    string outputTokensFileName;
    State state;
};

} // namespace token

#endif // TOKENIZER_H

