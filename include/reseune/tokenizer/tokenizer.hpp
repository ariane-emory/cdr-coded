#ifndef RESEUNE_TOKENIZER_HPP
#define RESEUNE_TOKENIZER_HPP

#include <tuple>

#include "reseune/util/util.hpp"
#include "reseune/c_str_cursor/c_str_cursor.hpp"
#include <cassert>

// ===============================================================================================================
// Macros
// ===============================================================================================================

#define ABORT                 {RESTORE; RETURN_NO_MATCH;}
#define HERE                  (**this)
#define MATH_OPS              X('+'), X('-'), X('/'), X('*'), X('%')      
#define MATCH                 CALL_MATCH_F(MF)
#define NEXT                  ((*this)++)
#define NO_MATCH               (span{nullptr, nullptr, false})
#define NUL_HERE              (0 == HERE)
#define POS                   (m_position)
#define RESTORE               (POS = saved)
#define REWIND                (POS = start)
#define SAVE                  MARK(saved)
#define SPAN                  span{start, POS}
#define START                 log("Entering %s.", __FUNCTION__); MARK(start); span match{NO_MATCH};
#define RETURN_MATCH          {log("Returning match after moving %zu from %s.", POS - start, __FUNCTION__);  return match;}
#define RETURN_NO_MATCH       {log("Returning no_match after moving %zu from %s.", POS - start, __FUNCTION__); return NO_MATCH;}
#define RETURN_SPAN           {log("Returning span after moving %zu from %s.", POS - start, __FUNCTION__); return SPAN;}
#define RETURN_EMPTY          {log("Returning empty after moving %zu from %s.", POS - start, __FUNCTION__); return span{POS, POS};}

#define T_CHAR_F              template <char_f CF>
#define T_MATCH_F             template <match_f MF>
#define T_2_MATCH_F           template <match_f LEFT_MF, match_f RIGHT_MF>

#define CALL_MATCH_F(match_f) {indentation += 2; match = (this->*match_f)(); indentation -= 2;}
#define MARK(name)            const char * const name{POS}; std::ignore = name
#define unless(expr)          if (! (expr))
#define until(expr)           while (! (expr))

#define CHAR_F(name)          constexpr inline static bool name(const char c)
#define MATCH_F(name, ...)    constexpr inline span name(__VA_ARGS__)
#define FROM_C_CHAR_F(name, fun)                                                \
  MATCH_F(name) {                                                               \
    log("About to enter '%s'...", # name);                                      \
    return character_f<fun>();                                                  \
  }                                                                             \
  MATCH_F(name ## s) {                                                          \
    return plus<&t::name>();                                                    \
  }                                                                             \
  MATCH_F(star_ ## name ## s) {                                                 \
    return star<&t::name>();                                                    \
  }

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
    constexpr tokenizer(const char * const str) : c_str_cursor(str), indentation(0), verbose(true) {}
    
    // ===========================================================================================================
    // Forward declarations
    // ===========================================================================================================
    struct span;
    
  private:
    // These are both only used by the log method below for debug output:
    size_t indentation;
    bool verbose;

    // ===========================================================================================================
    // Debug logging helper functions
    // ===========================================================================================================

    void log(const char * format, ...) const {
      unless (verbose)
        return;
      for (size_t ix = 0; ix < indentation; ix++)
        putchar(' ');
      va_list arglist;
      va_start(arglist, format);
      vprintf(format, arglist);
      va_end(arglist);
      putchar('\n');
    }
    
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
      RETURN_EMPTY;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(strip) {
      // Match against MF while ignoring any surrounding whitespace (before and after).
      START;
      if (NUL_HERE) 
        RETURN_NO_MATCH;
      ignore_whitespace();
      MATCH;
      if (NUL_HERE)
        RETURN_MATCH;
      ignore_whitespace();
      RETURN_MATCH;
    }

    // =============================================================================================================
    template<LABEL_T L, match_f MF>
    MATCH_F(label) {
      // Match against MF and, if it matches, label the token type of the resulting span as L.
      START;
      log("Would label as '%u'", L);
      MATCH;
      unless (match)
        RETURN_NO_MATCH;
      log("Label as '%u' was moved", L);
      match.label = L;
      RETURN_MATCH;      
    }

    // =============================================================================================================
    T_2_MATCH_F MATCH_F(both_of) {
      // Match against LEFT_MF and, if it matched, match against RIGHT_MF.
      START;
      CALL_MATCH_F(LEFT_MF);
      unless (match)
        goto fail;
      CALL_MATCH_F(RIGHT_MF);
      unless (match)
        goto fail;
      RETURN_SPAN;
    fail:
      REWIND;
      RETURN_NO_MATCH;
    }

    // =============================================================================================================
    template <match_f MF, match_f... MFs>
    MATCH_F(all_of) {
      // Match MF followed by the MFs in order.
      START;
      MATCH;
      unless (match)
        RETURN_NO_MATCH;
      const span rest = all_of<MFs...>();
      if (! rest)
        RETURN_NO_MATCH;
      RETURN_SPAN;
    }

    template <typename... nil>
    MATCH_F(all_of) {
      START;
      RETURN_EMPTY;
    }

    // =============================================================================================================
    T_2_MATCH_F MATCH_F(either_of) {
      // Match against LEFT_MF and, if it did not match, match against RIGHT_MF.
      return any_of<LEFT_MF, RIGHT_MF>();
    }

    // =============================================================================================================
    template <match_f MF, match_f... MFs>
    MATCH_F(any_of) {
      // Match against any of the MFs, attempting them from LEFT_MF to RIGHT_MF.
      START;
      MATCH;
      if (match)
        RETURN_MATCH;
      return any_of<MFs...>();
    }

    template <typename... nil>
    MATCH_F(any_of) {
      START;
      RETURN_NO_MATCH;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(star) {
      // Match against MF zero or more times.
      START;
      // const char * last_pos;
      do {
        // last_pos = POS;
        MATCH;
      } until (NUL_HERE || (! match));
      RETURN_SPAN;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(plus) {
      // Match against MF one or more times.
      START;
      match = both_of<MF, &t::star<MF>>();
      RETURN_MATCH;
    }

    // =============================================================================================================
    T_CHAR_F MATCH_F(character_f) {
      // Match a C-style char predicate function CF.

      //We will never, ever, permit maching a null character here!
      // If you want to do that you ought write some new function.
      // But you probably ought not want to.
      START;      
      if (NUL_HERE)
        return NO_MATCH;
      unless (CF(HERE)) {
        log("character_f did not match '%c'.", HERE, HERE);
        RETURN_NO_MATCH;
      }
      log("character_f matched '%c' (%u).", HERE, HERE);
      NEXT;      
      RETURN_SPAN;
    }

    // =============================================================================================================
    template <char C>
    MATCH_F(character) {
      // Match a particular character C.
      log("Compare '%c' (%u) with '%c' (%u).", HERE, HERE, C, C);
      return character_f<ischar<C>>();
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(zero_padded) {
      // Ignore any number of 0s and then match against MF.
      SAVE;
      ignore<&t::star<&t::character<'0'>>>();
      START;
      MATCH;
      unless (match)
        ABORT;
      RETURN_MATCH;
    }

    // =============================================================================================================
    MATCH_F(positive_integer) {
      // Match a positive integer. Does not permit a leading '+'!
      return either_of<
        &t::zero_padded<&t::digits>,
        &t::plus<&t::character<'0'>>>();
    }
    
    // =============================================================================================================
    MATCH_F(basic_math_op) {
      // Match basic math ops.
#define X(c) &t::tokenizer::character<c>
      return any_of<MATH_OPS>();
#undef X
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(optional) {
      // Match against MF and if it returns NO_MATCH, return an empty span instead.
      START;
      MATCH;
      if (match)
        RETURN_MATCH;
      RETURN_EMPTY;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(followed_by) {
      // Match against MF and if it returns a match, rewind and return empty.
      START;
      MATCH;
      unless (match)
        RETURN_NO_MATCH;
      REWIND;
      RETURN_EMPTY;
    }

// =============================================================================================================
    MATCH_F(integer) {
      // Match any integer (with or without leading zeroes).
      return both_of<
        &t::optional<
          &t::any_of<
            &t::character<'-'>,
            &t::character<'+'>>>,
        &t::digits>();
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
        &t::both_of<
          &t::basic_math_op,
          &t::followed_by<&t::whitespace>>,
        &t::both_of<
          &t::alpha,
          &t::star<&t::either_of<&t::character<'-'>,
                                 &t::alnum>>>>();
    }

    // =============================================================================================================
    // Convenience match functions
    // =============================================================================================================
    MATCH_F(ignore_whitespace) {
      // Ignore any number of whitespace characers.
      return ignore<&t::star_whitespaces>();
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
      bool matched;
      LABEL_T label;

      // ===========================================================================================================
      // Constructors
      // ===========================================================================================================
      constexpr span(
        const char * bb = nullptr,
        const char * ee = nullptr,
        bool mm = true,
        LABEL_T ll = static_cast<LABEL_T>(0)) : begin(bb), end(ee), matched(mm), label(ll) {}
      
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
      constexpr bool no_match() const {
        // True iff the span's beginning and end are both nullptr.
        return ! matched;
      }

      // ===========================================================================================================
      constexpr char * c_str() const {
        // Return a *NEW* C string containing the string that matched. THE CALLER OWNS THE RETURNED C STRING!
        return create_new_c_str(*this);
      }
      
      // ===========================================================================================================
      constexpr operator bool() const {
        // True iff the span is not empty.
        return ! no_match();
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
// Don't leak the macros!
// =================================================================================================================
#undef ABORT
#undef HERE
#undef MATCH
#undef NEXT
#undef NO_MATCH
#undef NUL_HERE
#undef POS
#undef RESTORE
#undef REWIND
#undef SAVE
#undef SPAN
#undef START
#undef RETURN_MATCH
#undef RETURN_NO_MATCH
#undef RETURN_SPAN

#undef T_CHAR_F
#undef T_MATCH_F
#undef T_2_MATCH_F

#undef CALL_MATCH_F
#undef MARK
#undef unless
#undef until

#undef CHAR_F
#undef MATCH_F
#undef FROM_C_CHAR_F

#undef MATH_OPS

#endif
