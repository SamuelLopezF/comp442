#include "../include/tokenizer.h"
#include <fstream>
#include <iostream>
#include <regex>

using namespace token;
using namespace std;

// A regex to match one or more whitespace or newline characters.
regex whitespace_or_newline("^[\\s\\n]+");

Tokenizer::Tokenizer(string filename) {
        // Assign the parameter to the member variable.
        this->filename = filename;

        // Initialize state values.
        state.new_state = 0;
        state.letter_start = 0;
        state.non_zero_digit_start = 0;
        state.zero_start = 0;
        state.special_char_start = 0;
        state.single_char_operator_start = 0;
        state.single_line_comment_start = 0;
        state.multi_line_comment_start = 0;
        state.interger_start = 0;
        state.fraction_start = 0;
        state.two_char_operator_start = 0;

        // Mark the state as started.
        state.new_state = 1;

        outputErrorsFileName = "./errors/" + filename + ".outlexerrors";
        outputTokensFileName = "./output/" + filename + ".outlextokens";
        setupOutputFile();
}

// IngestChar now returns a Lexeme.
// If whitespace is found, it returns a token of type "whitespace" along with the length.
// Otherwise, it calls NewToken.
Lexeme Tokenizer::IngestChar(const char *c) {
        cmatch match;
        regex whitespace_or_newline("^[\\s\\n]+");
        if (regex_search(c, match, whitespace_or_newline)) {
                // Create a Lexeme with token type "whitespace" and the matched string.
                Token t;
                t.type = "whitespace";
                t.value = match.str();
                Lexeme lex;
                lex.token = t;
                lex.length = match.length();
                return lex;
        } else {
                return NewToken(c);
        }
}

// NewToken returns a Lexeme containing the token and the length consumed.
Lexeme Tokenizer::NewToken(const char *c) {
        // Define regex patterns anchored at the start.
        regex id_token("^[a-zA-Z][a-zA-Z0-9_]*\\b");
        regex interger_token("^(0|[1-9][0-9]*)\\b");
        regex float_token("^(0|[1-9][0-9]*)(\\.[0-9]+)?(e[+-]?[0-9]+)?\\b");
        // Fraction: a dot followed by one or more digits.
        regex fraction("^\\.[0-9]+\\b");
        regex single_line_comment("^//.*\\n");
        // Multi-line comment: matches comments that span multiple lines.
        regex multi_line_comment("^/\\*[\\s\\S]*?\\*/");
        // Operator pattern.
        // // In tokenizer.cpp, update the operators regex pattern
        regex operators("^(==|<>|<=|>=|:=|=>|\\+|\\-|\\*|\\/|<|>|=|\\(|\\)|\\{|\\}|\\[|\\]|;|:|,|\\.)");
        // Reserved words.
        regex reserved("^(and|or|not|if|then|else|while|for|return|void|self|class|attribute|constructor|float|int|isa|read|write|put|public|private|local|function|implementation|program)\\b");


        cmatch match;
        Lexeme lex;
        if (regex_search(c, match, single_line_comment)) {
                cout << "[single-line comment , " << match.str() << "]" << endl;
                writeTokens("single-line comment", match.str());
                lex.token.type = "single-line comment";
                lex.token.value = match.str();
                lex.length = match.length();
                return lex;
        } else if (regex_search(c, match, multi_line_comment)) {
                cout << "[multi-line comment , " << match.str() << "]" << endl;
                writeTokens("multi-line comment", match.str());
                lex.token.type = "multi-line comment";
                lex.token.value = match.str();
                lex.length = match.length();
                return lex;
        } else if (regex_search(c, match, fraction)) {
                cout << "[fraction , " << match.str() << "]" << endl;
                writeTokens("fraction", match.str());
                lex.token.type = "fraction";
                lex.token.value = match.str();
                lex.length = match.length();
                return lex;
        } else if (regex_search(c, match, operators)) {
                cout << "[operator , " << match.str() << "]" << endl;
                writeTokens("operator", match.str());
                lex.token.type = "operator";
                lex.token.value = match.str();
                lex.length = match.length();
                return lex;
        } else if (regex_search(c, match, id_token)) {
                if (regex_match(match.str(), reserved)) {
                        cout << "[reserved , " << match.str() << "]" << endl;
                        writeTokens("reserved", match.str());
                        lex.token.type = "reserved";
                        lex.token.value = match.str();
                        lex.length = match.length();
                        return lex;
                } else {
                        cout << "[id , " << match.str() << "]" << endl;
                        writeTokens("id", match.str());
                        lex.token.type = "id";
                        lex.token.value = match.str();
                        lex.length = match.length();
                        return lex;
                }
        } else if (regex_search(c, match, interger_token)) {
                cout << "[integer , " << match.str() << "]" << endl;
                writeTokens("integer", match.str());
                lex.token.type = "integer";
                lex.token.value = match.str();
                lex.length = match.length();
                return lex;
        } else if (regex_search(c, match, float_token)) {
                cout << "[float , " << match.str() << "]" << endl;
                writeTokens("float", match.str());
                lex.token.type = "float";
                lex.token.value = match.str();
                lex.length = match.length();
                return lex;
        } else {
                cerr << "[Bad token : [" << c << "]" << endl;
                writeErrors(c);
                lex.token.type = "error";
                lex.token.value = c;
                lex.length = 1; // Consume at least one character.
                return lex;
        }
}

void Tokenizer::setupOutputFile() {
        ofstream tokens(outputTokensFileName, ios::app);
        ofstream errors(outputErrorsFileName, ios::app);

        if (!tokens.is_open()) {
                cerr << "ERROR opening token file " << filename + ".tokens" << endl;
        }
        if (!errors.is_open()) {
                cerr << "ERROR opening token file " << filename + ".errors" << endl;
        }
}

void Tokenizer::writeTokens(const string &tokenType, const string &tokenValue) {
        ofstream file(outputTokensFileName, ios::app);
        if (file.is_open()) {
                file << "[" << tokenType << ", " << tokenValue << "]" << endl;
                file.close();
        } else {
                cerr << "Error opening file for appending: " << outputTokensFileName << endl;
        }
}

void Tokenizer::writeErrors(const string &error) {
        ofstream file(outputErrorsFileName, ios::app);
        if (file.is_open()) {
                file << "[error, " << error << "]" << endl;
                file.close();
        } else {
                cerr << "Error opening file for appending: " << outputErrorsFileName << endl;
        }
}
