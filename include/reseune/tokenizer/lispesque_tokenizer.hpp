#ifndef RESEUNE_LISPESQUE_TOKENIZER_HPP
#define RESEUNE_LISPESQUE_TOKENIZER_HPP

#include "tokenizer.hpp"

#define BASES_MATCH_F(name) static constexpr span_t(tokenizer<tt>::*name)()
#define rule static constexpr match_f

#include "macros.hpp" // include last!

// =====================================================================================================================
namespace reseune {
  
  // ===================================================================================================================
  // Lispesque tokenizer class
  // ===================================================================================================================
  struct lispesque_tokenizer : public tokenizer<lispesque_token_type> {
    using tt     = lispesque_token_type;
    using base_t = tokenizer<tt>;
    using t      = lispesque_tokenizer;

    // =================================================================================================================
    // Constructors
    // =================================================================================================================
    constexpr lispesque_tokenizer(const char * const str) : base_t(str) {}

    // =================================================================================================================
    // Main terminal
    // =============================================================================================================
    virtual MATCH_F(token) {
      // Match many lispesque tokens.
      START;
      MATCH;
      RETURN_MATCH;
    }

  private:

    // Declare this rule a little early since we're going to use it while making primitives:
    rule lispesque_token_terminator =
      my any<my whitespace,
             my character<')'>>;
    
    // =================================================================================================================
    // Pointers to match_f templates in base.
    // =================================================================================================================
    T_MATCH_F BASES_MATCH_F(terminated)   = my all<MF, my followed_by<    lispesque_token_terminator>>;
    T_MATCH_F BASES_MATCH_F(unterminated) = my all<MF, my not_followed_by<lispesque_token_terminator>>;

    template <char... Cs>
    BASES_MATCH_F(terminated_word)        = terminated<my word<Cs...>>;
    
    // =================================================================================================================
    // Manufacture match_fs for common operator-like symbols as terminated_words as well as some primitive symbols.
    // =================================================================================================================
#define X(...) terminated_word<__VA_ARGS__>
#define Y(name) rule name = my any<name ## s>
    Y(boolean_op);
    Y(increment_decrement_op);
    Y(other_comparison_op);
    Y(other_math_op);
    Y(primitive_comparison_op);
    Y(primitive_math_op);
    Y(primitive_symbol);
#undef Y
#undef X
    
    // =================================================================================================================
    // Grammar rules.
    // =================================================================================================================
    rule lispesque_operator = my any<
      boolean_op,
      other_math_op,
      other_comparison_op,
      increment_decrement_op>;

    rule keyword_separator = my characters<'-'>;

    rule lispesque_primitive =
      my any<primitive_math_op,
             primitive_comparison_op,
             primitive_symbol>;

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

    rule MF =
      my strip<
      my any<my label<l_paren,     my character<'('>>,
             my label<r_paren,     terminated<my character<')'>>>,
             my label<quote,       unterminated<my character<'\''>>>,
             my label<tt::integer, terminated<my integer>>,
             my label<primitive,   lispesque_primitive>,
             my label<keyword,     lispesque_keyword>,
             my label<symbol,      lispesque_symbol>>>;
    
  public:
    
    // =================================================================================================================
  };
  // ===================================================================================================================
}
// =====================================================================================================================

// =====================================================================================================================
// Don't leak the macros!
// =====================================================================================================================
#undef bases
#undef BASES_MATCH_F
#undef BASES_T_MATCH_F
#undef rule

#include "undef_macros.hpp"
// =====================================================================================================================
#endif

