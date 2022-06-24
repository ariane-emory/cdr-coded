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
    constexpr lispesque_tokenizer(const char * const str) : base_t(str) {}

  protected:

    // =================================================================================================================
    // Types
    // =================================================================================================================
    using base_t = BASE_T;
    using tt     = base_t::label_type;
    using t      = lispesque_tokenizer;
    using span   = BASE_T::span_type;
    
    // =================================================================================================================
    // Pointers to match_f templates in base
    // =================================================================================================================

#define BASES_MATCH_F(name) static constexpr span (base_t::*name)()

    // 'Rename' a bunch of functions from base to names that will make the written grammar read more nicely. The
    // 'renamed' entities exist as static match_f *s.

#define RENAME(type, from, to)                                                  \
    template <type... Args>                                                     \
    BASES_MATCH_F(to) = my template from<Args...>
    
    RENAME(char,    character,   Char);
    RENAME(char,    characters,  Chars);
    RENAME(base_t::match_f, all,         All);
    RENAME(base_t::match_f, any,         Any);
    RENAME(base_t::match_f, star,        Star);
    RENAME(base_t::match_f, plus,        Plus);
    RENAME(base_t::match_f, optional,    Optional);
    RENAME(base_t::match_f, intercalate, Intercalate);
    RENAME(base_t::match_f, strip,       Strip);
#undef RENAME

    template <base_t::label_type L, base_t::match_f MF>
    BASES_MATCH_F(Label) = my template label<L, MF>;

    // Declare these rules a little early since we're going to use it while making the termination-related templates:
    static constexpr base_t::match_f Whitespace                 = my whitespace;
    static constexpr base_t::match_f Lispesque_Token_Terminator = Any<Whitespace, Char<')'>>;
    
    template <base_t::match_f MF> BASES_MATCH_F(Terminated)   = my template all<MF, my template followed_by    <Lispesque_Token_Terminator>>;
    template <base_t::match_f MF> BASES_MATCH_F(Unterminated) = my template all<MF, my template not_followed_by<Lispesque_Token_Terminator>>;
    template <char... Cs>
    
    BASES_MATCH_F(Terminated_Word)        = Terminated<my template word<Cs...>>;
    
#undef BASES_MATCH_F

    // =================================================================================================================
    // Manufacture match_f *s for common operator-like symbols as terminated_words as well as some primitive symbols.
    // =================================================================================================================
#define X(...) Terminated_Word<__VA_ARGS__>
#define Y(name) static constexpr base_t::match_f name = Any<name ## s>
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
    static constexpr base_t::match_f AlNums           = my alnums;
    static constexpr base_t::match_f Star_AlNums      = my star_alnums;
    static constexpr base_t::match_f Digits           = my digits;
    static constexpr base_t::match_f Star_Digits      = my star_digits;
    static constexpr base_t::match_f XDigits          = my xdigits;
    static constexpr base_t::match_f Star_XDigits     = my star_xdigits;
    static constexpr base_t::match_f Whitespaces      = my whitespaces;
    static constexpr base_t::match_f Star_Whitespaces = my star_whitespaces;
    static constexpr base_t::match_f Pos_Integer      = my integer;

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
