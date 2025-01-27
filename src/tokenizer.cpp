#include "../include/tokenizer.h"
#include <cstddef>
#include <iostream>
#include <regex>
#include <string>

using namespace token;
using namespace std;

regex whitespace_or_newline("[\s\n]");


Tokenizer::Tokenizer() {
  // layer one starts
  state.new_state = 0;
  state.letter_start = 0;
  state.non_zero_digit_start = 0;
  state.zero_start = 0;
  state.special_char_start = 0;
  state.single_char_operator_start = 0;
  state.single_line_comment_start = 0;
  state.multi_line_comment_start = 0;

  // layer two starts
  state.interger_start = 0;
  state.fraction_start = 0;
  state.two_char_operator_start = 0;

  // state status
  state.new_state = 1;
}

size_t Tokenizer::IngestChar(const char *c) {
  cmatch match;
  //regex whitespace_or_newline("[\\s\\n]");
    regex whitespace_or_newline("^[\\s\\n]");

  // Debug: Print current input character
  //std::cout << "Ingesting char: '" << *c << "'" << std::endl;

  if (regex_search(c, match, whitespace_or_newline)) {
    size_t offset =  match.length();
    //std::cout << "Skipping whitespace or newline, offset: " << offset << std::endl;
    return offset;
  } else {
    //std::cout << "New token detected" << std::endl;
    return NewToken(c);
  }
}

size_t Tokenizer::NewToken(const char *c) {
  // Define regex patterns
  regex whitespace_or_newline("[\\s\\n]");
  regex id_token("^[a-zA-Z][a-zA-Z0-9_]*(?=[\\s\\n]|\\z)");
  regex interger_token("^(0|[1-9][0-9]*)(?=[\\s\\n]|\\z)");
  regex float_token("^(0|[1-9][0-9]*)(\\.(0|[1-9][0-9]*))?(e[+-]?(0|[1-9][0-9]*))?(?=[\\s\\n]|\\z)");
  regex single_line_comment("^//.*\\n");
  regex multi_line_comment("/\\*.*?\\*/");
  regex operators("^(==|<>|<=|>=|:=|=>|\\+|\\-|\\*|\\/|<|>|\\(|\\)|\\{|\\}|\\[|\\]|;|:|,|\\.)(?=[\\s\\n]|\\z)");

  cmatch match;

  // Check for various token types
  if (regex_search(c, match, id_token)) {
    std::cout << "id token: " << match.str() << std::endl;
    return match.length();
  } else if (regex_search(c, match, interger_token)) {
    std::cout << "integer token: " << match.str() << std::endl;
    return match.length();
  } else if (regex_search(c, match, float_token)) {
    std::cout << "float token: " << match.str() << std::endl;
    return match.length();
  } else if (regex_search(c, match, single_line_comment)) {
    std::cout << "single-line comment token: " << match.str() << std::endl;
    return match.length();
  } else if (regex_search(c, match, multi_line_comment)) {
    std::cout << "multi-line comment token: " << match.str() << std::endl;
    return match.length();
  } else if (regex_search(c, match, operators)) {
    std::cout << "operator token: " << match.str() << std::endl;
    return match.length();
  }

  // If no match, return 1 to move the pointer forward
  return 1;
}


