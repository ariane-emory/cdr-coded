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
      return strip<
        my any<
          bases label<l_paren,     bases character<'('>>,
          bases label<r_paren,     bases with_lispesque_token_terminator<bases character<')'>>>,
          bases label<quote,       bases without_lispesque_token_terminator<bases character<'\''>>>,
          bases label<tt::integer, bases with_lispesque_token_terminator<bases integer>>,
          bases label<primitive,   bases lispesque_primitive>,
          bases label<keyword,     bases lispesque_keyword>,
          bases label<symbol,      bases lispesque_symbol>>>();

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

