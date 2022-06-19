#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdexcept>
#include <tuple>
#include "reseune/reseune.hpp"

// ===============================================================================================================

#define LINE        (reseune::print_line())
#define HLINE       (reseune::print_line('-'))
#define NEWLINE     (putchar('\n'))
#define PRINT(x, y) (print_bits<true,false>(x, uintptr(y)))
#define cout        (std::cout)
#define endl        (std::endl)
#define uintptr(x)  (reseune::uintptr(x))

// ===============================================================================================================

using namespace    reseune;

// ===============================================================================================================
int main() {
  enum label_t {
    unlabeled,
    l_paren,
    r_paren,
    word
  };
  
  using l = label_t;
  using t = tokenizer<l>;

  const char * const sexp { "(((abcdefg wo99 three four\n five six seven\n eight" };
  t                  tok  { sexp };
  t::span            result {};
  
  do {
    tok.ignore_whitespace();

    result = tok.strip<
      &t::either<
        &t::label<l_paren, &t::character<'('>>,
        &t::label<word,    &t::plain_symbol>>>();
    
    if (result.empty())
      printf("Word is null.\n");
    else
      printf("Word is(%u, '%s').\n", result.label, result.c_str());
  } while (! result.empty());
}
