#include "reseune/reseune.hpp"

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
  
  struct lisp_tokenizer : public reseune::tokenizer<token_type> {
    using b = reseune::tokenizer<token_type>;
    
    inline b::span lispesque_identifier() {
      return either<
        &b::basic_math_op,
        &b::both<
          &b::alpha,
          &b::star<&b::either<&b::character<'-'>,
                              &b::alnums>>>>();
    }
  };

  using t = lisp_tokenizer; 
  #define read &lisp_tokenizer::b::
 
  t                    tokenizer{input};
  lisp_tokenizer::span token{};
  size_t               token_num{1};
  
  do {
    token = tokenizer.strip<
      read either<
        read label<symbol, read lispesque_identifier>,
        read either <
          read label<integer, read integer>,
          read either<
            read label<l_paren, read character<'('>>,
            read label<r_paren, read character<')'>>>>>>();
    if (token) printf("Token #%zu is (token_type: %u, string: '%s').\n", token_num++, token.label, token.c_str());
  } while (token);
}
