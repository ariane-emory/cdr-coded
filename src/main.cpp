#include "reseune/reseune.hpp"

// ===============================================================================================================
#define tok &t::

// ===============================================================================================================
int main() {
  const char * const input {
    "b\n"
    "(+ 2 4)\n"
    "(* 2 581)\n"
    "(let (a (- 6 1)))\n"
    "(/ a (% a 3))\n"
    "5\n"
    "a\n"
  };
  
  enum token_type {
    unlabeled, // 0
    symbol,    // 1
    l_paren,   // 2
    r_paren,   // 3
    integer    // 4
  };
  
  using   t = reseune::tokenizer<token_type>;
  
  t       tokenizer {input};
  t::span token     {};
  size_t  token_num {1};
  
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
