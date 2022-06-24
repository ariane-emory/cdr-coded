#ifndef RESEUNE_LISPESQUE_TOKENIZER_HPP
#define RESEUNE_LISPESQUE_TOKENIZER_HPP

#include "tokenizer.hpp"

#include "macros.hpp" // include last!

// =====================================================================================================================
namespace reseune {  
  // ===================================================================================================================
  // Lispesque tokenizer abstract class
  // ===================================================================================================================
  struct lispesque_tokenizer : public tokenizer<tokenizer_span_with_children, lispesque_token_type> {

    // This class mostly just does some aliasing and forwarding of names so as to let the descendent class write it's
    // language's grammar in a more pleasant and readable way: it doesn't really do any work, it just renames stuff to
    // make the descendent classes nicer.
    //
    // It also defines a couple of basic terminals.
    //
    // If I decide to stick with the names established here, maybe some of this stuff will just get pushed down into their
    // base class, maybe. Not sure yet.

    // =================================================================================================================
    // Constructors
    // =================================================================================================================
    constexpr lispesque_tokenizer(const char * const str) : base_t(str) {}

  protected:

    // =================================================================================================================
    // Types
    // =================================================================================================================
    using base_t = tokenizer<tokenizer_span_with_children, lispesque_token_type>;
    using t      = lispesque_tokenizer;
    using tt     = base_t::label_type;

    // =================================================================================================================
    // Pointers to match_f templates in base
    // =================================================================================================================

    // 'Rename' a bunch of functions from base to names that will make the written grammar read more nicely. The
    // 'renamed' entities exist as static match_f *s.

#define BASES_MATCH_F(name) static constexpr span_type (base_t::*name)()
#define RENAME(type, from, to)                                                  \
    template <type... Args>                                                     \
    BASES_MATCH_F(to) = my from<Args...>
    
    RENAME(char,    character,   Char);
    RENAME(char,    characters,  Chars);
    RENAME(match_f, all,         All);
    RENAME(match_f, any,         Any);
    RENAME(match_f, star,        Star);
    RENAME(match_f, plus,        Plus);
    RENAME(match_f, optional,    Optional);
    RENAME(match_f, intercalate, Intercalate);
    RENAME(match_f, strip,       Strip);
#undef RENAME

    template <label_type L, match_f MF>
    BASES_MATCH_F(Label) = my label<L, MF>;

    // Declare these rules a little early since we're going to use it while making the termination-related templates:
    rule Whitespace                       = my whitespace;
    rule Lispesque_Token_Terminator       = Any<Whitespace, Char<')'>>;
    
    T_MATCH_F BASES_MATCH_F(Terminated)   = my all<MF, my followed_by    <Lispesque_Token_Terminator>>;
    T_MATCH_F BASES_MATCH_F(Unterminated) = my all<MF, my not_followed_by<Lispesque_Token_Terminator>>;
    template <char... Cs>    
    BASES_MATCH_F(Terminated_Word)        = Terminated<my word<Cs...>>;
#undef BASES_MATCH_F

    // =================================================================================================================
    // Manufacture match_f *s for common operator-like symbols as terminated_words as well as some primitive symbols.
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
    // Grammar production rules: Just give some rules prettier names.
    // =================================================================================================================
    rule AlNum             = my alnum;
    rule AlNums            = my alnums;
    rule Star_AlNums       = my star_alnums;

    rule Alpha             = my alpha;
    rule Alphas            = my alphas;
    rule Star_Alphas       = my star_alphas;

    rule Digit             = my digit;
    rule Digits            = my digits;
    rule Star_Digits       = my star_digits;

    rule XDigit            = my xdigit;
    rule XDigits           = my xdigits;
    rule Star_XDigits      = my star_xdigits;

    // Singular case is defined above.
    rule Whitespaces       = my whitespaces;
    rule Star_Whitespaces  = my star_whitespaces;

    rule NonZeroDigit      = Char<'1','2','3','4','5','6','7','8','9'>;
    rule NonZeroDigits     = Plus<NonZeroDigit>;
    rule StarNonZeroDigits = Star<NonZeroDigit>;

    rule ZeroDigit         = Char<'0'>;
    rule ZeroDigits        = Plus<ZeroDigit>;
    rule StarZeroDigits    = Star<ZeroDigit>;

    rule Pos_Integer       = my integer;

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
