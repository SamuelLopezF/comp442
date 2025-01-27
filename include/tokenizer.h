#ifndef TOKENIZER
#define TOKENIZER
#include <cstddef>
namespace token {
class Tokenizer {

  // Base
  struct Digit {
    char digit;
  };
  struct Letter {
    char letter;
  };
  struct ZeroDigit {
    char c;
  };
  struct Operator {
    char c;
  };
  struct AlphaNumeric {
    char c;
  };

  // Composed
  struct NonZeroDigit {
    Digit *digit;
  };
  struct Fraction {
    char dot;
    Digit *digits;
    NonZeroDigit end_digit;
  };
  struct Interger {
    Digit *digits;
  };

  struct Float {
    Interger interger;
    Fraction fraction;
    char exponent_char;
    char sign;
    Interger exponent;
  };

  struct Id {
    Letter id_start;
    AlphaNumeric *id;
  };

public:
  Tokenizer();
  Tokenizer(Tokenizer &&) = default;
  Tokenizer(const Tokenizer &) = default;
  Tokenizer &operator=(Tokenizer &&) = default;
  Tokenizer &operator=(const Tokenizer &) = default;
  ~Tokenizer() = default;
  size_t IngestChar(const char *c);
  size_t NewToken(const char *c);
  struct State {
    // layer one starts
    bool new_state;
    bool letter_start;
    bool non_zero_digit_start;
    bool zero_start;
    bool special_char_start;
    bool single_char_operator_start;
    bool single_line_comment_start;
    bool multi_line_comment_start;

    // layer two start
    bool interger_start;
    bool fraction_start;
    bool two_char_operator_start;
  };

private:
  State state;
};
} // namespace token
#endif // TOKENIZER
