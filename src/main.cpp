#include "reseune/reseune.hpp"

// ===============================================================================================================
using namespace    reseune;

enum label_t {
  unlabeled,
  l_paren,
  r_paren,
  word
};
  
using l = label_t;
using t = tokenizer<l>;

#define token &t::

// ===============================================================================================================
int main() {
  const char * const input    {"(((abcdefg wo99 three four\n five six seven\n eight" };
  t                  tokenizer{input};
  t::span            result   {};
  
  do {
    tokenizer.ignore_whitespace();

    result = tokenizer.strip<
      token either<
        token label<l_paren,   token character<'('>>,
        token either<
          token label<l_paren, token character<')'>>,
          token label<word,    token plain_symbol>>>>();
    
    if (result.empty())
      printf("Word is null.\n");
    else
      printf("Word is(%u, '%s').\n", result.label, result.c_str());
  } while (! result.empty());
}
