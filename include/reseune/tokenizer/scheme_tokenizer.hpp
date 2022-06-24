#ifndef RESEUNE_SCHEME_TOKENIZER_HPP
#define RESEUNE_SCHEME_TOKENIZER_HPP

#include "lispesque_tokenizer.hpp"
#include "macros.hpp" // include last!

// =====================================================================================================================
namespace reseune {
  
  // ===================================================================================================================
  // Lisp tokenizer class
  // ===================================================================================================================
  struct scheme_tokenizer : public lispesque_tokenizer {
    // =================================================================================================================
    // Constructors
    // =================================================================================================================
    constexpr scheme_tokenizer(const char * const str) : lispesque_tokenizer(str) {}

    // =================================================================================================================
    // Main terminal
    // =================================================================================================================
    virtual MATCH_F(token) {
      // Matchmost lispesque tokens.
      return (this->*Token)();
    }
    
  protected:
    
    // =================================================================================================================
    // Grammar production rules for a Scheme-like Lisp
    // =================================================================================================================
    rule Operator =
      Any<Boolean_Op,
          Other_Math_Op,
          Other_Comparison_Op,
          Increment_Decrement_Op>;

    rule Keyword_Separator =
      Chars<'-'>;

    rule Primitive =
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
      
    rule Keyword =
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
      
    rule Symbol =
      Label<symbol,
            Terminated<
              Any<Operator,
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
            Terminated<Pos_Integer>>;
      
    rule Token =
      Strip<
      Any<LParen,
          RParen,
          Quote,
          Integer,
          Primitive,
          Keyword,
          Symbol>>;

    // Rules that don't exist yet:
    //   Quasiquote
    //   Unquote
    
    // =================================================================================================================
  };
  // ===================================================================================================================
}
// =====================================================================================================================
// Don't leak the macros!
// =====================================================================================================================
#include "undef_macros.hpp"
// =====================================================================================================================
#endif

