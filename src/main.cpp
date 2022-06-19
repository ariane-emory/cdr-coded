#include "reseune/reseune.hpp"

// ===============================================================================================================
using namespace    reseune;

enum label_t {
  unlabeled,
  l_paren,
  r_paren,
  symbol
};
  
using l = label_t;
using t = tokenizer<l>;

#define tok &t::

// ===============================================================================================================
int main() {
  const char * const input    {"(((abcdefg wo_go_99 three four\n (five six) seven\n eight)" };
  t                  tokenizer{input};
  t::span            result   {};
  
  do {
    result = tokenizer.strip<
      tok either<
        tok label<symbol,    tok c_style_identifier>,
        tok either<
          tok label<l_paren, tok character<'('>>,
          tok label<r_paren, tok character<')'>>>>>();
    
    if (result)
      printf("Token is(%u, '%s').\n", result.label, result.c_str());
    else
      printf("Token is null.\n");
  } while (result);
}
