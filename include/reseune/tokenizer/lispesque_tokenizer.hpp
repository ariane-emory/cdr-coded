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
      // Match most lispesque tokens.
      START;
      MATCH;
      RETURN_MATCH;
    }

  private:

    // =================================================================================================================
    // Declare this rule a little early since we're going to use it while making primitives:
    // =================================================================================================================
    rule Lispesque_token_terminator =
      my any<my whitespace,
             my character<')'>>;
    
    // =================================================================================================================
    // Pointers to match_f templates in base.
    // =================================================================================================================
    T_MATCH_F BASES_MATCH_F(terminated)   = my all<MF, my followed_by<    Lispesque_token_terminator>>;
    T_MATCH_F BASES_MATCH_F(unterminated) = my all<MF, my not_followed_by<Lispesque_token_terminator>>;

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
    rule Lispesque_Operator = my any<
      boolean_op,
      other_math_op,
      other_comparison_op,
      increment_decrement_op>;

    rule Keyword_Separator = my characters<'-'>;

    rule Lispesque_Primitive =
      my label<primitive,
               my any<primitive_math_op,
                      primitive_comparison_op,
                      primitive_symbol>>;

    rule Symbol_Head =
      my all<my alpha,
             my star_alnums>;
      
    rule Keyword_Head =
      my all<my character<':'>,
             Symbol_Head>;

    rule Keyword_Body =
      my intercalate<Keyword_Head,
                     Keyword_Separator,
                     my alnums>;
      
    rule Lispesque_Keyword =
      my label<keyword,
               terminated<Keyword_Body>>;

    rule Symbol_Separator =
      my any<my characters<'-'>,
             my characters<':'>,
             my characters<'/'>>;
      
    rule Symbol_Body =
      my intercalate<Symbol_Head,
                     Symbol_Separator,
                     my alnums>;
      
    rule Symbol_Trailer = my optional<my character<'!','?'>>;
      
    rule Lispesque_Symbol =
      my label<symbol,
               terminated<my any<Lispesque_Operator,
                                 my all<Symbol_Body,
                                        Symbol_Trailer>>>>;

    rule LParen =
      my label<l_paren,
               my character<'('>>;

    rule RParen =
      my label<r_paren,
               terminated<my character<')'>>>;
    
    rule Token =
      my strip<
      my any<LParen,
             RParen,
             my label<quote,       unterminated<my character<'\''>>>,
             my label<tt::integer, terminated<my integer>>,
             Lispesque_Primitive,
             Lispesque_Keyword,
             Lispesque_Symbol>>;

    rule MF = Token;
    
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

