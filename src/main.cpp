#include "reseune/reseune.hpp"

// ===============================================================================================================
using namespace    reseune;

enum label_t {
  unlabeled,
  symbol,
  l_paren,
  r_paren,
  integer
};
  
using l = label_t;
using t = tokenizer<l>;

#define tok &t::

// ===============================================================================================================
int main() {
  // const char * const input    {"(((abcdefg wo-go-99 0 0 123 00123 1+ -12 -012 -33 three four\n (five six) seven\n eight 1+)" };
  const char * const input    {
    "b "
    "(+ 2 4) "
    "(* 2 581) "
    "(let (a (- 6 1))) "
    "(/ a (% a 3)) "
    "5 "
    "a "
  };
  
  t                  tokenizer{input};
  t::span            result   {};
  
  do {
    // result = tokenizer.strip<tok label<integer, tok integer>>();
    result = tokenizer.strip<
      tok either<
        tok label<symbol,  tok lispesque_identifier>,
          tok either <
            tok label<integer, tok integer>,
            tok either<
              tok label<l_paren, tok character<'('>>,
              tok label<r_paren, tok character<')'>>>>>>();
    
    if (result)
      printf("Token is (%u, '%s').\n", result.label, result.c_str());
    else
      printf("Token is null.\n");
  } while (result);
}
