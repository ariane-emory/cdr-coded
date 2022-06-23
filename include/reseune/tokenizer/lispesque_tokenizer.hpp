#ifndef RESEUNE_LISPESQUE_TOKENIZER_HPP
#define RESEUNE_LISPESQUE_TOKENIZER_HPP

#include "tokenizer.hpp"

#define bases &base::
#define BASES_T_MATCH_F(name) T_MATCH_F static constexpr span_t(base::*name)()

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

    BASES_T_MATCH_F(terminated)   = my with_lispesque_token_terminator<MF>;
    BASES_T_MATCH_F(unterminated) = my without_lispesque_token_terminator<MF>;
    
    // =================================================================================================================
    virtual MATCH_F(token) {
      // Match many lispesque tokens.

      constexpr match_f lispesque_primitive =
        my any<my primitive_math_op,
               my primitive_comparison_op,
               my primitive_symbol>;

      constexpr match_f keyword_head =
        my all<my character<':'>,
               my alpha,
               my star_alnums>;

      constexpr match_f keyword_separator = my characters<'-'>;

      constexpr match_f keyword_body =
        my intercalate<keyword_head,
                       keyword_separator,
                       my alnums>;
      
      constexpr match_f lispesque_keyword = terminated<keyword_body>;

      constexpr match_f symbol_head =
        my all<my alpha,
               my star_alnums>;
      
      constexpr match_f symbol_separator =
        my any<my characters<'-'>,
               my characters<':'>,
               my characters<'/'>>;
      
      constexpr match_f symbol_body =
        my intercalate<symbol_head,
                       symbol_separator,
                       my alnums>;
      
      constexpr match_f symbol_trailer = my optional<my character<'!','?'>>;
      
      constexpr match_f lispesque_symbol =
        terminated<my any<my lispesque_operator,
                          my all<symbol_body, symbol_trailer>>>;
      
      return strip<
        my any<
          my label<l_paren,     my character<'('>>,
          my label<r_paren,     terminated<my character<')'>>>,
          my label<quote,       unterminated<my character<'\''>>>,
          my label<tt::integer, terminated<my integer>>,
          my label<primitive,   lispesque_primitive>,
          my label<keyword,     lispesque_keyword>,
          my label<symbol,      lispesque_symbol>>>();
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
#undef BASES_T_MATCH_F

#include "undef_macros.hpp"
// =====================================================================================================================
#endif

