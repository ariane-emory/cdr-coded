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
    "(nil? var)\n"
    "(set! var -2)\n"
    "'(x:alpha/bang! 1)\n"
    "(vehicle :type 'car :brand 'honda :wheels 4)\n"
    "(++ a)\n"
    "(|| a b)\n"
    "(*= a 8)\n"
    "(let ((x 7)) x)\n"
  };

  using namespace    reseune; 
  using t          = tokenizer<lispesque_token_type>;
  t                  tokenizer{input};
  t::span_type       token{};
  size_t             token_num{1};

#define my &t::

  do {
    token = tokenizer.strip<
      my any<
        my label<l_paren,   my character<'('>>,
        my label<r_paren,   my with_lispesque_token_terminator<my character<')'>>>,
        my label<quote,     my without_lispesque_token_terminator<my character<'\''>>>,
        my label<integer,   my with_lispesque_token_terminator<my integer>>,
        my label<primitive, my lispesque_primitive>,
        my label<keyword,   my lispesque_keyword>,
        my label<symbol,    my lispesque_identifier>>>();
    if (token)
      printf("Token #%zu is (token_type: '%s', string: '%s').\n",
             token_num++,
             lispesque_token_type_strings[token.label],
             token.c_str());
  } while (token);

  printf("Input length: %zu.\n", strlen(input));
}

int main() {
  reseune::measure_time(&tokenize);
}

#undef read
