#ifndef RESEUNE_LISPESQUE_TOKENIZER_HPP
#define RESEUNE_LISPESQUE_TOKENIZER_HPP

#include "tokenizer.hpp"

#define bases &base::
#define BASES_T_MATCH_F(name) T_MATCH_F static constexpr span_t(base::*name)()
#define rule static constexpr match_f

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

  private:

#define X(...) my terminated_word<__VA_ARGS__>
// #define Y(name) MATCH_F(name) { return any<name ## s>(); }
#define Y(name) rule x__ ## name = my any<name ## s>
    
    // =================================================================================================================
    // Manufacture match_fs for common operator-like symbols as terminated_words.
    // =================================================================================================================
    Y(boolean_op);
    Y(increment_decrement_op);
    Y(other_comparison_op);
    Y(other_math_op);
    Y(primitive_comparison_op);
    Y(primitive_math_op);

    // Also for symbols likely to be primitives:
    Y(primitive_symbol);

#undef Y
#undef X

    
    // =================================================================================================================
    // Pointers to match_f templates in base.
    // =================================================================================================================
    BASES_T_MATCH_F(terminated)   = my all<MF, my followed_by<    my lispesque_token_terminator>>;
    BASES_T_MATCH_F(unterminated) = my all<MF, my not_followed_by<my lispesque_token_terminator>>;

    // =================================================================================================================
    // Grammar rules.
    // =================================================================================================================
    rule lispesque_operator = my any<
      my boolean_op,
      my other_math_op,
      my other_comparison_op,
      my increment_decrement_op>;

    rule keyword_separator = my characters<'-'>;

    rule lispesque_primitive =
      my any<my primitive_math_op,
             my primitive_comparison_op,
             my primitive_symbol>;

    rule symbol_head =
      my all<my alpha,
             my star_alnums>;
      
    rule keyword_head =
      my all<my character<':'>,
             symbol_head>;

    rule keyword_body =
      my intercalate<keyword_head,
                     keyword_separator,
                     my alnums>;
      
    rule lispesque_keyword = terminated<keyword_body>;

    rule symbol_separator =
      my any<my characters<'-'>,
             my characters<':'>,
             my characters<'/'>>;
      
    rule symbol_body =
      my intercalate<symbol_head,
                     symbol_separator,
                     my alnums>;
      
    rule symbol_trailer = my optional<my character<'!','?'>>;
      
    rule lispesque_symbol =
      terminated<my any<lispesque_operator,
                        my all<symbol_body,
                               symbol_trailer>>>;
    
  public:
    
    // =================================================================================================================
    virtual MATCH_F(token) {
      // Match many lispesque tokens.
      
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
#undef rule

#include "undef_macros.hpp"
// =====================================================================================================================
#endif

