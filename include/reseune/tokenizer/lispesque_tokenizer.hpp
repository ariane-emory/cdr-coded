#ifndef RESEUNE_LISPESQUE_TOKENIZER_HPP
#define RESEUNE_LISPESQUE_TOKENIZER_HPP

#include "tokenizer.hpp"
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

  protected:

    // =================================================================================================================
    // Pointers to match_f templates in base.
    // =================================================================================================================
    T_MATCH_F BASES_MATCH_F(Optional)     = my optional<MF>;
    template <match_f... MFs>
    BASES_MATCH_F(Intercalate)            = my intercalate<MFs...>;
    template <match_f... MFs>
    BASES_MATCH_F(Strip)                  = my strip<MFs...>;
    template <label_t L, match_f MF>
    BASES_MATCH_F(Label)                  = my label<L, MF>;
    template <match_f... MFs>
    BASES_MATCH_F(All)                    = my all<MFs...>;
    template <match_f... MFs>
    BASES_MATCH_F(Any)                    = my any<MFs...>;
    template <char... Cs>
    BASES_MATCH_F(Char)                   = my character<Cs...>;
    template <char... Cs>
    BASES_MATCH_F(Chars)                  = my characters<Cs...>;    

    // Declare these rules a little early since we're going to use it while making primitives:
    rule Whitespace                       = my whitespace;
    rule Lispesque_Token_Terminator       =
      Any<Whitespace,
          Char<')'>>;
    
    T_MATCH_F BASES_MATCH_F(Terminated)   = my all<MF, my followed_by<    Lispesque_Token_Terminator>>;
    T_MATCH_F BASES_MATCH_F(Unterminated) = my all<MF, my not_followed_by<Lispesque_Token_Terminator>>;
    template <char... Cs>
    BASES_MATCH_F(Terminated_Word)        = Terminated<my word<Cs...>>;

    // =================================================================================================================
    // Manufacture match_fs for common operator-like symbols as terminated_words as well as some primitive symbols.
    // =================================================================================================================
#define X(...) Terminated_Word<__VA_ARGS__>
#define Y(name) rule name = Any<name ## s>
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
    // Grammar rules: Just give some rules prettier names.
    // =================================================================================================================
    rule AlNums           = my alnums;
    rule Star_AlNums      = my star_alnums;
    rule Digits           = my digits;
    rule Star_Digits      = my star_digits;
    rule Whitespaces      = my whitespaces;
    rule Star_Whitespaces = my star_whitespaces;
    rule Pos_Integer      = my integer;

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

