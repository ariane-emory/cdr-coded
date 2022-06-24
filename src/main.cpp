#include "reseune/tokenizer/scheme_tokenizer.hpp"
#include "reseune/util/util.hpp"
#include "reseune/singly_linked_list/singly_linked_list.hpp"

// =====================================================================================================================
using namespace reseune; 

// =====================================================================================================================
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
    "1\n"
    "-1\n"
    "+1\n"
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

  using t          = scheme_tokenizer;
  t                  tokenizer{input};
  t::span_type       token{};
  size_t             token_num{};

#define my &t::

  do {
    tokenizer >> token;
    
    if (token)
      printf("Token #%zu is (token_type: '%s', string: '%s').\n",
             ++token_num,
             lispesque_token_type_strings[token.label],
             token.c_str());
  } while (token);

  printf("Input length: %zu.\n", strlen(input));
}

// =====================================================================================================================
void test_sll() {
  singly_linked_list<int> sll;

  printf("\nAdding things:\n");
  sll.add(1);
  sll.add(2);
  sll.add(3);
  printf("\n");
  sll.print();
  printf("\n");
  
  printf("Inserting a thing:\n");
  sll.print();
  printf("\n");
  
  // print_sll(sll);
  // sll.remove(0);

  // print_sll(sll);
  // sll.remove(0);

  // print_sll(sll);
  // sll.remove(0);

  // print_sll(sll);
  // sll.remove(0);

  printf("Done tests, length = %zu.\n", sll.size());
}

// =====================================================================================================================
int main() {
  // reseune::measure_time(&tokenize);
  test_sll();
}

