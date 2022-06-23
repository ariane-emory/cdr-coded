#ifndef RESEUNE_LISPESQUE_TOKENIZER_HPP
#define RESEUNE_LISPESQUE_TOKENIZER_HPP

#include "tokenizer.hpp"

#define bases &base::

#include "macros.hpp" // include last!

// =====================================================================================================================
namespace reseune {

  // ===================================================================================================================
  // Lispesque tokenizer class
  // ===================================================================================================================
  struct lispesque_tokenizer : public tokenizer<lispesque_token_type> {
    using tt   = lispesque_token_type;
    using base = tokenizer<tt>;
    using t    = lispesque_tokenizer;

    // =================================================================================================================
    // Constructors
    // =================================================================================================================
    constexpr lispesque_tokenizer(const char * const str) : base(str) {}
    
    // =================================================================================================================
    virtual MATCH_F(token) {
      // Match many lispesque tokens.

      constexpr match_f lispesque_primitive{my any<
        my primitive_math_op,
        my primitive_comparison_op,
        my primitive_symbol
        >};
      
      return strip<
        my any<
          my label<l_paren,     my character<'('>>,
          my label<r_paren,     my with_lispesque_token_terminator<my character<')'>>>,
          my label<quote,       my without_lispesque_token_terminator<my character<'\''>>>,
          my label<tt::integer, my with_lispesque_token_terminator<my integer>>,
          my label<primitive,   lispesque_primitive>,
          my label<keyword,     my lispesque_keyword>,
          my label<symbol,      my lispesque_symbol>>>();
    }
    // =================================================================================================================
  };
  // ===================================================================================================================
}
// =====================================================================================================================

// =====================================================================================================================
// Don't leak the macros!
// =====================================================================================================================
#undef bases
#include "undef_macros.hpp"
// =====================================================================================================================
#endif

