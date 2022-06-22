#ifndef RESEUNE_TOKENIZER_HPP
#define RESEUNE_TOKENIZER_HPP

#include <tuple>

#include "reseune/util/util.hpp"
#include "reseune/c_str_cursor/c_str_cursor.hpp"

// ===============================================================================================================
// Macros
// ===============================================================================================================
#define RESEUNE_VERBOSE_TOKENIZE 1

#ifdef RESEUNE_VERBOSE_TOKENIZE
#define VERBOSE            true
#else
#define VERBOSE            false
#endif

#define PRINTF(...)        { INDENT; if (VERBOSE) printf(__VA_ARGS__); }
#define PUTCHAR(c)         { INDENT; if (VERBOSE) putchar(c); }
#define INDENT             { for (size_t ix = 0; ix < indentation; ix++) PUTCHAR(' '); }

#define ABORT              {REWIND(restore); return NOTHING;}
#define CHAR_MATCHES       (CF(HERE))
#define MOVED              (start != POS)
#define HERE               (**this)
#define MATCH              { indentation += 2; match = {(this->*MF)()}; indentation -= 2; } 
#define NEXT               ((*this)++)
#define NOTHING            (span{})
#define NULL_HERE          (0 == HERE)
#define POS                (m_position)
#define SAVE               MARK(restore)
#define SPAN               span{start, POS}
#define START              MARK(start); span match{NOTHING}

#define unless(expr)       if (! (expr))
#define until(expr)        while (! (expr))
#define MARK(name)         const char * const name{POS}; std::ignore = name
#define REWIND(name)       (POS = name)
#define DO_MATCH(match_f)  match = {(this->*match_f)()}

#define T_CHAR_F           template <char_f CF>
#define T_CHAR             template <char C>
#define T_2_CHAR_F         template <char_f left, char_f right>
#define T_MATCH_F          template <match_f MF>
#define T_2_MATCH_F        template <match_f left, match_f right>

#define CHAR_F(name)       constexpr inline static bool name(const char c)
#define MATCH_F(name, ...) constexpr inline span name(__VA_ARGS__)

#define FROM_C_CHAR_F(name, fun)                                                \
  MATCH_F(name) {                                                               \
    return character_f<fun>();                                                  \
  }                                                                             \
  MATCH_F(name ## s) {                                                          \
    return plus<&t::name>();                                                    \
  }                                                                             \
  MATCH_F(star_ ## name ## s) {                                                 \
    return star<&t::name>();                                                    \
  }

// Not yet used, but I may want them in the future, maybe:
// #define BACK               (--*this)
// #define STASH              span stashed{match}
// #define UNSTASH            return stashed

// ===============================================================================================================
namespace reseune {
  
  // =============================================================================================================
  // tokenizer class
  // =============================================================================================================
  template <typename LABEL_T>
  class tokenizer : public c_str_cursor {
  public:

    // ===========================================================================================================
    // Constructors
    // ===========================================================================================================
    tokenizer(const char * const str) : c_str_cursor(str), indentation(0), verbose(true) {}
    
    // ===========================================================================================================
    // Forward declarations
    // ===========================================================================================================
    struct span;
    
  private:
    size_t indentation;
    bool verbose;
    // ===========================================================================================================
    // Types
    // ===========================================================================================================
    using char_f   = int (*)(int);
    using match_f  = span (tokenizer::*)();
    using t        = tokenizer;

  public:
    
    // =============================================================================================================
    // Match functions
    // =============================================================================================================
    T_MATCH_F MATCH_F(ignore) {
      // Match against MF and ignore the result.
      START;
      MATCH;
      return NOTHING;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(strip) {
      // Match against MF while ignoring any surrounding whitespace (before and after).
      ignore_whites();
      START;
      MATCH;
      ignore_whites();
      return match;
    }

    // =============================================================================================================
    template<LABEL_T L, match_f MF>
    MATCH_F(label) {
      // Match against MF and, if it matches, label the token type of the resulting span as L.
      START;
      MATCH;
      if MOVED
        match.label = L;
      return match;
    }

    // =============================================================================================================
    T_2_MATCH_F MATCH_F(both_of) {
      // Match against left and, if it matched, match against right.
      START;

      DO_MATCH(left);
      unless (MOVED)
        return NOTHING;

      DO_MATCH(right);
      unless (MOVED)
        return NOTHING;

      return SPAN;
    }

    // =============================================================================================================
    template <match_f MF, match_f... MFs>
    MATCH_F(all_of) {
      // Match all of the MFs in order.
      START;
      MATCH;
      unless (MOVED)
        return NOTHING;
      const span rest = all_of<MFs...>();
      if (rest == NOTHING)
        return NOTHING;
      return SPAN;
    }

    template <typename... nil>
    MATCH_F(all_of) {
      START;
      return SPAN; // An empty span (but not NOTHING!).
    }

    // =============================================================================================================
    T_2_MATCH_F MATCH_F(either_of) {
      // Match against left and, if it did not match, match against right.
      return any_of<left, right>();
    }

    // =============================================================================================================
    template <match_f MF, match_f... MFs>
    MATCH_F(any_of) {
      // Match against any of the MFs, attempting them from left to right.
      START;
      MATCH;
      if MOVED
        return match;
      return any_of<MFs...>();
    }

    template <typename... nil>
    MATCH_F(any_of) {
      return NOTHING;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(star) {
      // Match against MF zero or more times.
      START;
      const char * last_pos;
      do {
        last_pos = POS;
        MATCH;
      } until (NULL_HERE || POS == last_pos);
      return SPAN;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(plus) {
      // Match against MF one or more times.
      return both_of<MF, &t::star<MF>>();
    }

    // =============================================================================================================
    T_CHAR_F MATCH_F(character_f) {
      // Match a C-style char predicate function CF.

      //We will never, ever, permit maching a null character here!
      // If you want to do that you ought write some new function.
      // But you probably ought not want to.
      if (NULL_HERE)
        return NOTHING;
     
      START;      
      unless (CHAR_MATCHES)
        return NOTHING;
      NEXT;      
      return SPAN;
    }

    // =============================================================================================================
    T_CHAR MATCH_F(character) {
      // Match a particular character C.
      return character_f<ischar<C>>();
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(zero_padded) {
      // Ignore any number of 0s and then match against MF.
      SAVE;
      ignore<&t::star<&t::character<'0'>>>();
      START;
      MATCH;
      unless (MOVED)
        ABORT;
      return match;
    }

    // =============================================================================================================
    MATCH_F(positive_integer) {
      // Match a positive integer. Does not permit a leading '+' presently!
      return either_of<
        &t::zero_padded<&t::digits>,
        &t::plus<&t::character<'0'>>>();
    }
    
    // =============================================================================================================
    MATCH_F(basic_math_op) {
      // Match basic math ops.
#define c character
#define e either_of
      return any_of<
        &t::c<'+'>,
        &t::c<'-'>,
        &t::c<'/'>,
        &t::c<'*'>,
        &t::c<'%'>>();
#undef c
#undef e
    }

    // =============================================================================================================
    T_2_MATCH_F MATCH_F(optional_prefix) {
      // Match against left and then match against right (whether or not left moved the cursor).
      START;
      ignore<left>();
      DO_MATCH(right);
      return SPAN;
    }

    // =============================================================================================================
    MATCH_F(integer) {
      // Match any integer (with or without leading zeroes). Does not permit a leading '+' presently!
      return optional_prefix
        <&t::character<'-'>,
         &t::positive_integer>();
    }

    // =============================================================================================================
    MATCH_F(c_style_identifier) {
      // Match unqualified C-style identifiers. This should probably match all of them, I think?
      // If anything, it's too broad: it will accept '__' or '___', etc., I'm not immediately certain if those are
      // legal in #C.... those might be legal, but they're also /weird/. Whatever, we'll accept 'em for now.
      return both_of<
        &t::either_of<
          &t::character<'_'>,
          &t::alpha>,
        &t::plus<
          &t::either_of<&t::character<'_'>,
                        &t::alnums>>>();
    }

    // =============================================================================================================
    MATCH_F(lispesque_identifier) {
      // Match a subset of Lisp-style identifiers. Just a subset! Not all of them, yet.
      // Currently, identifiers must either_of:
      //   1. Consist of solely a basic math operator or,
      //   2. Begin with an alphabetic character and proceed with a sequence of alphanumeric characters and/or dashes.
      return either_of<
        &t::basic_math_op,
        &t::both_of<
          &t::alpha,
          &t::star<&t::either_of<&t::character<'-'>,
                                 &t::alnums>>>>();
    }

    // =============================================================================================================
    // Convenience match functions
    // =============================================================================================================
    MATCH_F(ignore_whites) {
      // Ignore any number of whitespace characers.
      return ignore<&t::whitespaces>();
    }    

    // Manufacture functions of type match_f corresponding to various C-style string predicate functions.
    FROM_C_CHAR_F(alnum,          isalnum);
    FROM_C_CHAR_F(alpha,          isalpha);
    FROM_C_CHAR_F(digit,          isdigit);
    FROM_C_CHAR_F(xdigit,         isxdigit);
    FROM_C_CHAR_F(whitespace,     iswhitespace);
    FROM_C_CHAR_F(non_whitespace, negate<iswhitespace>);

    // =============================================================================================================
    // Character predicate helper static functions
    // =============================================================================================================
    template <int (*fun)(int)>
    constexpr static int negate(int c) {
      // Make a negated version of a C-style character predicate function.
      return 0 == fun(c) ? 1 : 0;
    }
     
    // =============================================================================================================
    // Span struct
    // =============================================================================================================
    struct span {
      const char * begin;
      const char * end;
      LABEL_T label;

      // ===========================================================================================================
      // Constructors
      // ===========================================================================================================
      constexpr span(
        const char * bb = nullptr,
        const char * ee = nullptr,
        LABEL_T ll = static_cast<LABEL_T>(0)) : begin(bb), end(ee), label(ll) {}
      
      // ===========================================================================================================
      // Member functions
      // ===========================================================================================================
      constexpr size_t length() const {
        // Return the length of the span.
        return end - begin;
      }

      // ===========================================================================================================
      constexpr bool empty() const {
        // True iff the span's length is 0.
        return 0 == length();
      }

      // ===========================================================================================================
      constexpr char * c_str() const {
        // Return a *new* C string containing the string that matched. THE CALLER OWNS THE RETURNED C STRING!
        return create_new_c_str(*this);
      }
      
      // ===========================================================================================================
      constexpr operator bool() const {
        // True iff the span is not empty.
        return ! empty();
      }

      // ===========================================================================================================
    };

  private: 

    // =============================================================================================================
    // Static functions
    // =============================================================================================================
    constexpr static inline char * create_new_c_str(span const & tok) {
      // Create a *new* C string from a span. THE CALLER OWNS THE RETURNED C STRING!

      if (tok.empty()) return nullptr;
  
      const size_t siz  {(tok.length() + 1) * sizeof(char)};
      char * const word {static_cast<char *>(malloc(siz))};

      memcpy(word, tok.begin, siz);

      word[tok.length()] = 0;

      return word;
    }

    // =============================================================================================================
  };
  // ===============================================================================================================
}
// =================================================================================================================

// =================================================================================================================
// Don't leak macros!
// =================================================================================================================
#undef VERBOSE
#undef PRINTF
#undef PUTCHAR

#undef ABORT
#undef CHAR_MATCHES       
#undef MOVED              
#undef HERE               
#undef MATCH
#undef NEXT               
#undef NOTHING
#undef NULL_HERE
#undef POS
#undef SAVE
#undef SPAN
#undef START

#undef unless
#undef until
#undef MARK
#undef REWIND
#undef DO_MATCH

#undef T_CHAR_F
#undef T_CHAR
#undef T_2_CHAR_F
#undef T_MATCH_F
#undef T_2_MATCH_F

#undef CHAR_F
#undef MATCH_F

#undef FROM_C_CHAR_F

#endif
