#ifndef RESEUNE_LISPESQUE_TOKENIZER_HPP
#define RESEUNE_LISPESQUE_TOKENIZER_HPP

#include "tokenizer.hpp"
#include "macros.hpp" // include last!

// =====================================================================================================================
namespace reseune {
  
  // ===================================================================================================================
  // Lispesque tokenizer abstract class
  // ===================================================================================================================
  struct lispesque_tokenizer : public tokenizer<lispesque_token_type> {

    // This class mostly just does some aliasing and forwarding of names so as to let the descendant class write it's
    // language's grammar in a more pleasant and readable way: it doesn't really do any work, it just renames stuff to
    // make the descendant classes nicer.

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
    using tt     = lispesque_token_type;
    using base_t = tokenizer<tt>;
    using t      = lispesque_tokenizer;

    // =================================================================================================================
    // Pointers to match_f templates in base
    // =================================================================================================================
    template <label_t L, match_f MF>
    BASES_MATCH_F(Label)                  = my label<L, MF>;

    // 'Rename' a bunch of functions from base to more knames that will make the written grammar read more nicely. The
    // 'renamed' entities exist as static match_f *s.
#define RENAME(type, from, to)                                                  \
    template <type... Args>                                                     \
    BASES_MATCH_F(to) = my from<Args...>
    RENAME(match_f, optional,    Optional);
    RENAME(match_f, intercalate, Intercalate);
    RENAME(match_f, strip,       Strip);
    RENAME(match_f, all,         All);
    RENAME(match_f, any,         Any);
    RENAME(match_f, star,        Star);
    RENAME(match_f, plus,        Plus);
    RENAME(char,    character,   Char);
    RENAME(char,    characters,  Chars);
#undef RENAME

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
    // Grammar production rules: Just give some rules prettier names.
    // =================================================================================================================
    rule AlNums           = my alnums;
    rule Star_AlNums      = my star_alnums;
    rule Digits           = my digits;
    rule Star_Digits      = my star_digits;
    rule XDigits          = my xdigits;
    rule Star_XDigits     = my star_xdigits;
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

