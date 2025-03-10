#include "../include/ast.h"
#include "../include/ast_builder.h"
#include "../include/filereader.h"
#include "../include/parser.h"
#include "../include/tokenizer.h"
#include <cstddef>
#include <iostream>
#include <regex>
#include <stdexcept>
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
        try {
                // Build and (optionally) print the parsing table.
                map<string, map<string, string>> table = buildParsingTable();
                for (auto &nonterm : table) {
                        cout << "Nonterminal: " << nonterm.first << "\n";
                        for (auto &entry : nonterm.second) {
                                cout << "    Terminal: " << entry.first << "  ==>  Production: " << entry.second << "\n";
                        }
                        cout << "\n";
                }

                // Initialize the incremental parser state.
                initParserState();

                // Get the file path from command-line arguments.
                string filepath = parse_args(argc, argv, "-f");
                if (filepath.empty()) {
                        filepath = "./examples/example1.source"; // default file if none provided.
                }

                try {
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
                        vector<Token> tokens; // Store all tokens for AST building

                        // Process the file one token at a time.
                        bool parsingSucceeded = true;
                        while (remaining > 0) {
                                // IngestChar returns a Lexeme containing a token and the number of characters consumed.
                                Lexeme lex = token_engine.IngestChar(content);
                                content += lex.length;
                                remaining -= lex.length;

                                // Filter out tokens that are not part of the grammar (comments and whitespace).
                                if (lex.token.type == "whitespace" || lex.token.type == "single-line comment" || lex.token.type == "multi-line comment") {
                                        continue;
                                }

                                // Store valid token
                                tokens.push_back(lex.token);

                                // Feed the token to the parser.
                                if (!feedToken(lex.token)) {
                                        cout << "Parsing halted due to syntax error." << endl;
                                        parsingSucceeded = false;
                                        break;
                                }
                        }

                        // If parsing was successful, build and print the AST
                        if (parsingSucceeded && remaining == 0) {
                                cout << "Parsing completed successfully. Building AST..." << endl;

                                try {
                                        // Add this line to build the AST
                                        buildASTFromTokens();

                                        // Then get the root from the AST builder module
                                        ASTNode *ast = getASTRoot();
                                        if (ast != nullptr) {
                                                // Print the AST to a file
                                                string astOutputFile = "./output/" + filepath + ".ast";
                                                printAST(ast, astOutputFile);
                                                cout << "AST has been written to " << astOutputFile << endl;
                                        } else {
                                                cout << "Failed to build AST: AST root is null" << endl;
                                        }
                                } catch (const std::exception &e) {
                                        cerr << "Exception while building/printing AST: " << e.what() << endl;
                                }
                        }
                } catch (const std::exception &ex) {
                        cerr << "Error reading file: " << ex.what() << endl;
                        return 1;
                }

                return 0;
        } catch (const std::exception &ex) {
                cerr << "Error: " << ex.what() << endl;
                return 1;
        }
}
