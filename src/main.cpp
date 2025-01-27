#include <iostream>
#include <regex>
#include "./filereader.cpp"
#include "../include/tokenizer.h"
#include <string>

using namespace token;
using namespace std; 
int main() {
  try {
    // Initialize the reader with a file path
    string filepath = "./assignment1.COMP442-6421.paquet.2025.4/lexnegativegrading.src";
    //string filepath = "./test_cases/test1.src";
    FileCharReader reader(filepath);

    // Get a pointer to the file content
    const char *content = reader.getCharPointer();
    size_t remaining = reader.size();

    // Regex pattern for alphanumeric words
    // std::regex pattern("[a-zA-Z0-9]+");
    // std::cmatch match;
    Tokenizer token_engine = Tokenizer();

    while (remaining > 0) {
        //if (std::regex_search(content, match, pattern)) {
        // std::cout << "Matched: " << match.str() << std::endl;
        //
        // // Move the pointer past the matched portion
        // size_t offset = match.position() + match.length();
            //
        size_t offset = token_engine.IngestChar(content);
        content += offset;
        remaining -= offset;
    }
        return 0;
  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
  }
  return 0;
}
