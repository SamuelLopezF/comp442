#include "../include/parser.h"
#include "../include/tokenizer.h"
#include "../include/filereader.h"
#include "../include/ast_builder.h"
#include <cstddef>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
using namespace token;
using namespace std;

string parse_args(int argc, char **argv, const string &flag) {
    string filepath;
    for (int i = 1; i < argc - 1; ++i) {
        if (string(argv[i]) == flag) {
            filepath = argv[i + 1];
            break;
        }
    }
    return filepath;
}

int main(int argc, char **argv) {
    // Create AST builder
    ASTBuilder astBuilder;
    
    // Initialize the incremental parser state with AST builder
    initParserState(&astBuilder);

    try {
        // Get the file path from command-line arguments.
        string filepath = parse_args(argc, argv, "-f");
        if (filepath.empty()) {
            filepath = "./examples/example2.source"; // default file if none provided.
        }
        FileCharReader reader(filepath);

        // Remove any unwanted substring ("input/") from the filepath.
        string substring = "input/";
        size_t pos = string::npos;
        while ((pos = filepath.find(substring)) != string::npos) {
            filepath.erase(pos, substring.length());
        }

        // Get a pointer to the file content.
        const char *content = reader.getCharPointer();
        size_t remaining = reader.size();

        Tokenizer token_engine = Tokenizer(filepath);

        // Process the file one token at a time.
        while (remaining > 0) {
            // IngestChar returns a Lexeme containing a token and the number of characters consumed.
            Lexeme lex = token_engine.IngestChar(content);
            content += lex.length;
            remaining -= lex.length;

            // Filter out tokens that are not part of the grammar (comments and whitespace).
            if (lex.token.type == "whitespace" ||
                lex.token.type == "single-line comment" ||
                lex.token.type == "multi-line comment")
            {
                continue;
            }

            // Feed the token to the parser.
            if (!feedToken(lex.token)) {
                cout << "Parsing halted due to syntax error." << endl;
                break;
            }
        }
        
        // Add an EOF token to complete parsing
        Token eofToken;
        eofToken.type = "$";
        eofToken.value = "$";
        feedToken(eofToken);
        
        // Get the generated AST and print it
        cout << "\n=== Abstract Syntax Tree ===\n";
        astBuilder.printAST();
        
        return 0;
    } catch (const std::exception &ex) {
        cerr << "Error: " << ex.what() << endl;
    }

    return 0;
}
