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

#define tok &t::

// ===============================================================================================================
int main() {
  const char * const input    {"(((abcdefg wo99 three four\n (five six) seven\n eight)" };
  t                  tokenizer{input};
  t::span            result   {};
  
  do {
    result = tokenizer.strip<
      tok either<
        tok label<l_paren,   tok character<'('>>,
        tok either<
          tok label<r_paren, tok character<')'>>,
          tok label<word,    tok plain_symbol>>>>();
    
    if (result)
      printf("Word is(%u, '%s').\n", result.label, result.c_str());
    else
      printf("Word is null.\n");
  } while (result);
}
