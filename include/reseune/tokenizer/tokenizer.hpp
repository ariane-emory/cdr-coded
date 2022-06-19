#ifndef RESEUNE_TOKENIZER_HPP
#define RESEUNE_TOKENIZER_HPP

#include <tuple>

#include "reseune/util/util.hpp"
#include "reseune/c_str_cursor/c_str_cursor.hpp"

#define CHAR_MATCHES       (CF(HERE))
#define MOVED              (start != POS)
#define HERE               (**this)
#define MATCH              match = {(this->*MF)()}
#define NEXT               ((*this)++)
#define NOTHING            (span{})
#define NULL_HERE          (0 == HERE)
#define POS                (m_position)         
#define SPAN               span{start, POS}
#define START              MARK(start); span match{NOTHING}
#define T_CHAR_F           template <char_f CF>
#define T_2_CHAR_F         template <char_f left, char_f right>
#define T_MATCH_F          template <match_f MF>
#define T_2_MATCH_F        template <match_f left, match_f right>
#define CHAR_F(name)       inline static bool name(const char c)
#define DO_MATCH(match_f)  match = {(this->*match_f)()}
#define MARK(name)         const char * const name{POS}; std::ignore = name
#define MATCH_F(name, ...) inline span name(__VA_ARGS__)
#define REWIND(name) (POS = name)
#define unless(expr)       if (! (expr))
#define until(expr)        while (! (expr))
#define FROM_C_CHAR_F(name, fun)                                                \
  MATCH_F(name) {                                                               \
    return character_f<fun>();                                                  \
  }                                                                             \
  MATCH_F(name ## s) {                                                          \
return plus<&t::name>();                                                        \
}                                                                               \
MATCH_F(star_ ## name ## s) {                                                   \
  return star<&t::name>();                                                      \
}

// Not yet used, but I may want them in the future, maybe:
// #define BACK               (--*this)
// #define STASH              span stashed{match}

// =================================================================================================================
namespace reseune {
  
  // ===============================================================================================================
  template <typename LABEL_T>
  class tokenizer : public c_str_cursor {
  public:

    // ===============================================================================================================
    struct span;
    
  private:

    // ===============================================================================================================
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
      // Match against match_f and ignore the result.
      START;
      MATCH;
      return NOTHING;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(strip) {
      // Match against match_f while ignoring any surrounding whitespace (before and after).
      ignore_whites();
      START;
      MATCH;
      ignore_whites();
      return match;
    }

    // =============================================================================================================
    template<LABEL_T L, match_f MF>
    MATCH_F(label) {
      // Match against match_f and, if it matches label the token type of the resulting span as L.
      START;
      MATCH;
      if MOVED
        match.label = L;
      return match;
    }

    // =============================================================================================================
    T_2_MATCH_F MATCH_F(both) {
      // Match against left and, if it matched, match against right.
      START;
      DO_MATCH(left);
      unless (MOVED)
        return NOTHING;
      DO_MATCH(right); 
      return SPAN;
    }

    // =============================================================================================================
    T_2_MATCH_F MATCH_F(either) {
      // Match against left and, if it did not match, match against right.
      START;
      DO_MATCH(left);
      if MOVED
        return match;
      DO_MATCH(right);
      if MOVED
        return match;
      return NOTHING;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(star) {
      // Match against match_f zero or more times.
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
      // Match against match_f one or more times
      return both<MF, &t::star<MF>>();
    }

    // =============================================================================================================
    T_CHAR_F MATCH_F(character_f) {
      // Match a C-style char predicate function.

      //We will never, ever, permit maching a null character here!
      // If you want to do that you ought write some other function.
      if (NULL_HERE)
        return NOTHING;
      
      START;      
      unless (CHAR_MATCHES)
        return NOTHING;
      NEXT;      
      return SPAN;
    }

    // =============================================================================================================
    template <char C>
    MATCH_F(character) {
      // Match a particular character.
      return character_f<ischar<C>>();
    }

    // =============================================================================================================
    // Convenience match functions
    // =============================================================================================================
    MATCH_F(ignore_whites) {
      // Ignore any number of whitespace characers.
      return ignore<&t::whitespaces>();
    }    

    // Manufacture functions of type match_f for various C-style string predicate functions.
    FROM_C_CHAR_F(alnum,          isalnum);
    FROM_C_CHAR_F(alpha,          isalpha);
    FROM_C_CHAR_F(digit,          isdigit);
    FROM_C_CHAR_F(xdigit,         isxdigit);
    FROM_C_CHAR_F(whitespace,     iswhitespace);
    FROM_C_CHAR_F(non_whitespace, negate<iswhitespace>);

    // =============================================================================================================
    T_MATCH_F MATCH_F(zero_padded) {
      // Ignore any number of 0s and then match against MF.
      MARK(restore);
      ignore<&t::star<&t::character<'0'>>>();
      START;
      MATCH;
      unless (MOVED) {
        REWIND(restore);
        return NOTHING;
      }
      return match;
    }

    // =============================================================================================================
    MATCH_F(positive_integer) {
      // Match a positive integer (but without a leading '+', for now).
      return either<
        &t::zero_padded<&t::digits>,
        &t::plus<&t::character<'0'>>>();
    }

    // =============================================================================================================
    MATCH_F(basic_math_op) {
      // Match basic math ops.
      return either<&t::character<'+'>,
                    &t::either<&t::character<'-'>,
                               &t::either<&t::character<'/'>,
                                          &t::either<&t::character<'*'>,
                                                     &t::character<'%'>>>>>();
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
      // Match any integer (with or without leading zeroes).
      return optional_prefix<&t::character<'-'>, &t::positive_integer>();
    }

    // =============================================================================================================
    MATCH_F(c_style_identifier) {
      // Match C-style identifiers.
      return both<&t::either<&t::character<'_'>, &t::alpha>, &t::star<&t::either<&t::character<'_'>, &t::alnums>>>();
    }

    // =============================================================================================================
    MATCH_F(lispesque_identifier) {
      // Match a subset of Lisp-style identifiers. Just a subset! Not all of them, yet.
      return either<
        &t::basic_math_op,
        &t::both<
          &t::alpha,
          &t::star<&t::either<&t::character<'-'>,
                              &t::alnums>>>>();
    }

    // =============================================================================================================
    // Character predicate helper static functions
    // =============================================================================================================
    template <int (*fun)(int)>
    static int negate(int c) {
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

      span(
        const char * bb = nullptr,
        const char * ee = nullptr,
        LABEL_T ll = static_cast<LABEL_T>(0)) : begin(bb), end(ee), label(ll) {}
      
      size_t length() const {
        // Return the length of the span.
        return end - begin;
      }

      bool empty() const {
        // True iff the span's length is 0.
        return 0 == length();
      }

      operator bool() const {
        // True iff the span is not empty.
        return ! empty();
      }

      char * c_str() const {
        // Return a *new* C string containing the string that matched. YOU own the C string!
        return create_new_c_str(*this);
      }
    };

  private: 

    // =============================================================================================================
    // Static functions
    // =============================================================================================================
    static inline char * create_new_c_str(span const & tok) {
      // Create a *new* C string from a span. The caller owns the returned C string!

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

#undef CHAR_MATCHES
#undef MOVED
#undef HERE
#undef MATCH
#undef NEXT
#undef NOTHING
#undef NULL_HERE
#undef POS
#undef REWIND
#undef SPAN
#undef START
#undef T_CHAR_F
#undef T_2_CHAR_F
#undef T_MATCH_F
#undef T_2_MATCH_F

#undef CHAR_F
#undef DO_MATCH
#undef MARK
#undef MATCH_F
#undef unless
#undef until
#undef FROM_C_CHAR_F

#endif
