#ifndef RESEUNE_LISPESQUE_TOKENIZER_HPP
#define RESEUNE_LISPESQUE_TOKENIZER_HPP

#include "tokenizer.hpp"
#include "macros.hpp" // include last!

// =====================================================================================================================
namespace reseune {  
  // ===================================================================================================================
  // Lispesque tokenizer abstract class
  // ===================================================================================================================
  template <typename BASE_T>
  struct lispesque_tokenizer : public BASE_T {

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
    constexpr lispesque_tokenizer(const char * const str) : BASE_T(str) {}

  protected:

    // =================================================================================================================
    // Types
    // =================================================================================================================
    using label_type = BASE_T::label_type;
    using match_f    = tokenizer<label_type>::match_f;
    using span       = BASE_T::span_type;
    using t          = lispesque_tokenizer;
    
    // =================================================================================================================
    // Pointers to match_f templates in base
    // =================================================================================================================

#define BASES_MATCH_F(name) static constexpr span (BASE_T::*name)()

    // 'Rename' a bunch of functions from base to names that will make the written grammar read more nicely. The
    // 'renamed' entities exist as static match_f *s.

#define RENAME(type, from, to)                                                  \
    template <type... Args>                                                     \
    BASES_MATCH_F(to) = my template from<Args...>
    
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
    BASES_MATCH_F(Label) = my template label<L, MF>;

    // Declare these rules a little early since we're going to use it while making the termination-related templates:
    rule Whitespace                 = my whitespace;
    rule Lispesque_Token_Terminator = Any<Whitespace, Char<')'>>;
    
    template <match_f MF> BASES_MATCH_F(Terminated)      = my template all<MF, my template followed_by    <Lispesque_Token_Terminator>>;
    template <match_f MF> BASES_MATCH_F(Unterminated)    = my template all<MF, my template not_followed_by<Lispesque_Token_Terminator>>;
    template <char... Cs> BASES_MATCH_F(Terminated_Word) = Terminated<my template word<Cs...>>;
    
#undef BASES_MATCH_F

    // =================================================================================================================
    // Manufacture match_f *s for common operator-like symbols as terminated_words as well as some primitive symbols.
    // =================================================================================================================
#define X(...) Terminated_Word<__VA_ARGS__>
#define Y(name) static constexpr match_f name = Any<name ## s>
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
