#include "reseune/tokenizer/tokenizer.hpp"
#include "reseune/util/util.hpp"

// ===============================================================================================================
void tokenize() {
  const char * const input {
    "'b\n"
    "2\n"
    "(+ 2 4)\n"
    "'(* 2 581)\n"
    "(let (a (- 6 1)))\n"
    "(/ a (% a 3))\n"
    "5\n"
    "0\n"
    "-0\n"
    "+0\n"
    "01\n"
    "-01\n"
    "+01\n"
    "a\n"
    "-250\n"
    "(dolist (mode\n"
    "'(beacon-mode\n"
    "tooltip-mode\n"
    "window-divider-mode))\n"
    "(funcall mode -1)\n"
    "(setq-default mode nil))\n"
  };
  
  enum token_type {
    unlabeled, // 0
    l_paren,   // 1
    r_paren,   // 2
    symbol,    // 3
    integer,   // 4
    quote,     // 5
  };
  
  using t = reseune::tokenizer<token_type>;
  t         tokenizer{input};
  t::span   token{};
  size_t    token_num{1};
  
  #define read &t::
  
  do {
    token = tokenizer.strip<
      read any_of<
        read label<quote,   read character<'\''>>,
        read label<l_paren, read character<'('>>,
        read label<r_paren, read character<')'>>,
        read label<integer, read integer>,
        read label<symbol,  read lispesque_identifier>>>();
    if (token)
      printf("Token #%zu is (token_type: %u, string: '%s').\n", token_num++, token.label, token.c_str());
  } while (token);
}

int main() {
  reseune::measure_time(&tokenize);
}

#undef read
