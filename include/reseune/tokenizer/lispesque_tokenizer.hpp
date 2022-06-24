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
    rule Lispesque_Token_Terminator =
      __ any<__ whitespace,
             __ character<')'>>;
    
    // =================================================================================================================
    // Pointers to match_f templates in base.
    // =================================================================================================================
    T_MATCH_F BASES_MATCH_F(Terminated)   = __ all<MF, __ followed_by<    Lispesque_Token_Terminator>>;
    T_MATCH_F BASES_MATCH_F(Unterminated) = __ all<MF, __ not_followed_by<Lispesque_Token_Terminator>>;
    T_MATCH_F BASES_MATCH_F(Optional)     = __ optional<MF>;
    template <char... Cs>
    BASES_MATCH_F(TerminatedWord)         = Terminated<__ word<Cs...>>;
    template <match_f... MFs>
    BASES_MATCH_F(Intercalate)            = __ intercalate<MFs...>;
    template <match_f... MFs>
    BASES_MATCH_F(Strip      )            = __ strip<MFs...>;
    template <label_t L, match_f MF>
    BASES_MATCH_F(Label)                  = __ label<L, MF>;
    template <match_f... MFs>
    BASES_MATCH_F(All)                    = __ all<MFs...>;
    template <match_f... MFs>
    BASES_MATCH_F(Any)                    = __ any<MFs...>;
    template <char... Cs>
    BASES_MATCH_F(Char)                   = __ character<Cs...>;
    template <char... Cs>
    BASES_MATCH_F(Chars)                  = __ characters<Cs...>;    

    // =================================================================================================================
    // Manufacture match_fs for common operator-like symbols as terminated_words as well as some primitive symbols.
    // =================================================================================================================
#define X(...) TerminatedWord<__VA_ARGS__>
#define Y(name) rule name = __ any<name ## s>
    Y(Boolean_Op);
    Y(Increment_Decrement_Op);
    Y(Other_Comparison_Op);
    Y(Other_Math_Op);
    Y(Primitive_Comparison_Op);
    Y(Primitive_Math_Op);
    Y(Primitive_Symbol);
#undef Y
#undef X
    
    // =================================================================================================================
    // Grammar rules 1/2: Just give some rules prettier names.
    // =================================================================================================================
    rule AlNums     = __ alnums;
    rule StarAlNums = __ star_alnums;
    rule PosInteger = __ integer;

    // =================================================================================================================
    // Grammar rules 2/2: Main grammar
    // =================================================================================================================
    rule Lispesque_Operator =
      Any<Boolean_Op,
          Other_Math_Op,
          Other_Comparison_Op,
          Increment_Decrement_Op>;

    rule Keyword_Separator =
      Chars<'-'>;

    rule Lispesque_Primitive =
      Label<primitive,
            Any<Primitive_Math_Op,
                Primitive_Comparison_Op,
                Primitive_Symbol>>;

    rule Symbol_Head =
      All<AlNums>;
      
    rule Keyword_Head =
      All<Char<':'>,
          Symbol_Head>;

    rule Keyword_Body =
      Intercalate<Keyword_Head,
                  Keyword_Separator,
                  AlNums>;
      
    rule Lispesque_Keyword =
      Label<keyword,
            Terminated<Keyword_Body>>;

    rule Symbol_Separator =
      Any<Chars<'-'>,
          Chars<':'>,
          Chars<'/'>>;

    rule Symbol_Body =
      Intercalate<Symbol_Head,
                  Symbol_Separator,
                  AlNums>;
      
    rule Symbol_Trailer = Optional<Char<'!','?'>>;
      
    rule Lispesque_Symbol =
      Label<symbol,
            Terminated<Any<Lispesque_Operator,
                           All<Symbol_Body,
                               Symbol_Trailer>>>>;

    rule LParen =
      Label<l_paren,
            Char<'('>>;

    rule RParen =
      Label<r_paren,
            Terminated<Char<')'>>>;

    rule Quote =
      Label<quote,
            Unterminated<Char<'\''>>>;

    rule Integer =
      Label<tt::integer,
            Terminated<PosInteger>>;
      
    rule Token =
      Strip<Any<LParen,
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

