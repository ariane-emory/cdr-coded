#include "reseune/reseune.hpp"

// ===============================================================================================================
using namespace reseune;

// ===============================================================================================================
#define tok &t::

// ===============================================================================================================
int main() {
  enum l {
    unlabeled,
    symbol,
    l_paren,
    r_paren,
    integer
  };
  
  using t = tokenizer<l>;

  const char * const input    {
    "b\n"
    "(+ 2 4)\n"
    "(* 2 581)\n"
    "(let (a (- 6 1)))\n"
    "(/ a (% a 3))\n"
    "5\n"
    "a\n"
  };
  
  t       tokenizer{input};
  t::span token   {};
  size_t  token_num{1};
  
  do {
    token = tokenizer.strip<
      tok either<
        tok label<symbol,  tok lispesque_identifier>,
        tok either <
          tok label<integer, tok integer>,
          tok either<
              tok label<l_paren, tok character<'('>>,
              tok label<r_paren, tok character<')'>>>>>>();
    
    if (token)
      printf("Token #%zu is (%u, '%s').\n", token_num++, token.label, token.c_str());
    else
      printf("Token #zu is null.\n", token_num++);
  } while (token);
}
