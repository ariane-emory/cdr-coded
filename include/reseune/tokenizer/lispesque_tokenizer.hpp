#ifndef RESEUNE_LISPESQUE_TOKENIZER_HPP
#define RESEUNE_LISPESQUE_TOKENIZER_HPP

#include "tokenizer.hpp"

#define BASES_MATCH_F(name) static constexpr span_t(tokenizer<tt>::*name)()
#define rule static constexpr match_f
#define __ my

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
      __ any<__ whitespace,
             __ character<')'>>;
    
    // =================================================================================================================
    // Pointers to match_f templates in base.
    // =================================================================================================================
    T_MATCH_F BASES_MATCH_F(terminated)   = __ all<MF, __ followed_by<    Lispesque_token_terminator>>;
    T_MATCH_F BASES_MATCH_F(unterminated) = __ all<MF, __ not_followed_by<Lispesque_token_terminator>>;

    template <char... Cs>
    BASES_MATCH_F(terminated_word)        = terminated<__ word<Cs...>>;
    
    // =================================================================================================================
    // Manufacture match_fs for common operator-like symbols as terminated_words as well as some primitive symbols.
    // =================================================================================================================
#define X(...) terminated_word<__VA_ARGS__>
#define Y(name) rule name = __ any<name ## s>
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
    rule Lispesque_Operator = __ any<
      boolean_op,
      other_math_op,
      other_comparison_op,
      increment_decrement_op>;

    rule Keyword_Separator = __ characters<'-'>;

    rule Lispesque_Primitive =
      __ label<primitive,
               __ any<primitive_math_op,
                      primitive_comparison_op,
                      primitive_symbol>>;

    rule Symbol_Head =
      __ all<__ alpha,
             __ star_alnums>;
      
    rule Keyword_Head =
      __ all<__ character<':'>,
             Symbol_Head>;

    rule Keyword_Body =
      __ intercalate<Keyword_Head,
                     Keyword_Separator,
                     __ alnums>;
      
    rule Lispesque_Keyword =
      __ label<keyword,
               terminated<Keyword_Body>>;

    rule Symbol_Separator =
      __ any<__ characters<'-'>,
             __ characters<':'>,
             __ characters<'/'>>;
      
    rule Symbol_Body =
      __ intercalate<Symbol_Head,
                     Symbol_Separator,
                     __ alnums>;
      
    rule Symbol_Trailer = __ optional<__ character<'!','?'>>;
      
    rule Lispesque_Symbol =
      __ label<symbol,
               terminated<__ any<Lispesque_Operator,
                                 __ all<Symbol_Body,
                                        Symbol_Trailer>>>>;

    rule LParen =
      __ label<l_paren,
               __ character<'('>>;

    rule RParen =
      __ label<r_paren,
               terminated<__ character<')'>>>;

    rule Quote =
      __ label<quote,
               unterminated<__ character<'\''>>>;

    rule Integer =
      __ label<tt::integer,
               terminated<__ integer>>;
      
    rule Token =
      __ strip<
      __ any<LParen,
             RParen,
             Quote,
             Integer,
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
#undef __

#include "undef_macros.hpp"
// =====================================================================================================================
#endif

