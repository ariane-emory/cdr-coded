#ifndef RESEUNE_TOKENIZER_HPP
#define RESEUNE_TOKENIZER_HPP

#include <tuple>
#include <cassert>

#include "reseune/util/util.hpp"
#include "reseune/c_str_cursor/c_str_cursor.hpp"

#include "tokenizer_span.hpp"
#include "macros.hpp" // include last!

// =====================================================================================================================
namespace reseune {

  // ===================================================================================================================
  // tokenizer class
  // ===================================================================================================================
  template <typename LABEL_T>
  class tokenizer {
  public:

    // =================================================================================================================
    // Constructors
    // =================================================================================================================
    constexpr tokenizer(const char * const str) : cursor(str), indentation(0), verbose(false) {}
    
    // =================================================================================================================
    // Public types
    // =================================================================================================================
    using span_type = tokenizer_span<LABEL_T>;

  private:
    c_str_cursor cursor;
    
    // These are both only used by the log method below for debug output:
    size_t       indentation;
    bool         verbose;

    // =================================================================================================================
    // Debug logging helper functions
    // =================================================================================================================

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
    
    // =================================================================================================================
    // Private types
    // =================================================================================================================
    using char_f   = int (*)(int);
    using match_f  = tokenizer_span<LABEL_T> (tokenizer::*)();
    using t        = tokenizer;

  public:
    
    // ===================================================================================================================
    // Match function templates
    // ===================================================================================================================
    T_MATCH_F MATCH_F(star) {
      // Match against MF zero or more times.
      START;
      do {
        MATCH;
      } until (NUL_HERE || (! match));
      RETURN_SPAN;
    }

    // ===================================================================================================================
    T_MATCH_F MATCH_F(plus) {
      // Match against MF one or more times.
      START;
      constexpr auto mf {my all<MF, my star<MF>>};
      CALL_MATCH_F(mf);
      RETURN_MATCH;
    }

    // ===================================================================================================================
    T_MATCH_F MATCH_F(zero_padded) {
      // Ignore any number of 0s and then match against MF. This needs a non-greedy version.
      ignore<my star<my character<'0'>>>();
      START;
      MATCH;
      MAYBE_RETURN_MATCH;
      REWIND;
      RETURN_NO_MATCH;
    }

    // ===================================================================================================================
    T_MATCH_F MATCH_F(optional) {
      // Match against MF and if it returns NO_MATCH, return an empty span instead.
      START;
      MATCH;
      MAYBE_RETURN_MATCH;
      RETURN_EMPTY;
    }

    // ===================================================================================================================
    T_MATCH_F MATCH_F(followed_by) {
      // Match against MF, rewind, and if it returned a match, return empty.
      START;
      MATCH;
      REWIND;
      MAYBE_RETURN_NO_MATCH;
      RETURN_EMPTY;
    }

    // ===================================================================================================================
    T_MATCH_F MATCH_F(not_followed_by) {
      // Match against MF, rewined, and if it returned a match, return NO_MATCH, otherwise return empty.
      return followed_by<my negate<MF>>();
    }

    // ===================================================================================================================
    T_MATCH_F MATCH_F(negate) {
      // Match against MF and if it returns a match, and return NO_MATCH, otherwise return empty.
      START;
      MATCH;
      unless (match)
        RETURN_EMPTY;
      RETURN_NO_MATCH;
    }

    // ===================================================================================================================
    T_MATCH_F MATCH_F(ignore) {
      // Match against MF and ignore the result by returning empty.
      // Should this ever return NO_MATCH? Maybe...
      START;
      MATCH;
      RETURN_EMPTY;
    }

    // ===================================================================================================================
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
      unless (match)
        REWIND;
      RETURN_MATCH;
    }

    // ===================================================================================================================
    T_MATCH_F MATCH_F(with_lispesque_token_terminator) {
      return all<
        MF,
        my followed_by<
          my lispesque_token_terminator>>();
    }
    
    // ===================================================================================================================
    T_MATCH_F MATCH_F(without_lispesque_token_terminator) {
      return all<
        MF,
        my not_followed_by<
          my lispesque_token_terminator>>();
    }
    
    // ===================================================================================================================
    template <char_f CF>
    MATCH_F(character_f) {
      // Match a C-style char predicate function CF.

      // We will never, ever, permit maching a null character here!
      // If you want to do that you ought write some new function.
      // But you probably ought not want to.
      START;      
      if (NUL_HERE)
        RETURN_NO_MATCH;
      unless (CF(HERE)) {
        if (verbose) {
          if ('\n' == HERE)
            log("character_f did not match '\\n'.");
          else
            log("character_f did not match '%c'.", HERE);
        }
        RETURN_NO_MATCH;
      }
      if (verbose) {
        if ('\n' == HERE)
          log("character_f matched '\\n'.");
        else
          log("character_f matched '%c'.", HERE);
      }
      NEXT;      
      RETURN_SPAN;
    }

    // ===================================================================================================================
    template<LABEL_T L, match_f MF>
    MATCH_F(label) {
      // Match against MF and, if it matches, label the token type of the resulting span as L.
      START;
      log("Would label as '%u'", L);
      MATCH;
      MAYBE_RETURN_NO_MATCH;
      log("Label as '%u' was matched.", L);
      match.label = L;
      RETURN_MATCH;      
    }

    // ===================================================================================================================
    template <match_f MF, match_f... MFs>
    MATCH_F(all) {
      // Match MF followed by the MFs in order.
      START;
      MATCH;
      MAYBE_RETURN_NO_MATCH;
      constexpr auto mf {my all<MFs...>};
      CALL_MATCH_F(mf);
      if (match)
        RETURN_SPAN;
      REWIND;
      RETURN_NO_MATCH;
    }
    
    template <typename... nil>
    MATCH_F(all) {
      START;
      RETURN_EMPTY;
    }

    // ===================================================================================================================
    template <match_f MF, match_f... MFs>
    MATCH_F(any) {
      // Match against any of the MFs, attempting them from LEFT_MF to RIGHT_MF.
      START;
      MATCH;
      MAYBE_RETURN_MATCH;
      constexpr auto mf {my any<MFs...>};
      CALL_MATCH_F(mf);
      RETURN_MATCH;
    }
    
    template <typename... nil>
    MATCH_F(any) {
      START;
      RETURN_NO_MATCH;
    }

    // ===================================================================================================================
    template <char C>
    MATCH_F(character) {
      // Match a particular character C.
      START;
      log("Compare '%c' (%u) with '%c' (%u).", HERE, HERE, C, C);
      constexpr auto mf {my character_f<ischar<C>>};
      CALL_MATCH_F(mf);
      RETURN_MATCH;
    }

    // ===================================================================================================================
    // Convenience match functions
    // ===================================================================================================================
    MATCH_F(lispesque_token_terminator) {
      return any<
        my whitespace,
        my character<')'>>();
    }
    
    // ===================================================================================================================
    MATCH_F(positive_integer) {
      // Match a positive integer. Does not permit a leading '+'!
      return any<
        my zero_padded<my digits>,
        my plus<my character<'0'>>>();
    }
    
    // ===================================================================================================================
    MATCH_F(integer) {
      // Match any integer (with or without leading zeroes).
      return all<
        my optional<
          my any<
            my character<'-'>,
            my character<'+'>>>,
        my digits>();
    }

#define X(c) my tokenizer::character<c>

    // ===================================================================================================================
    MATCH_F(basic_math_op) {
      // Match basic math ops.
      return any<BASIC_MATH_OPS>();

    }

    // ===================================================================================================================
    MATCH_F(basic_comparison_op) {
      // Match basic comparison ops.
      return any<BASIC_COMPARISON_OPS>();
    }

#undef X
#define X(A,B) my all<my character<A>,my character<B>>

    // ===================================================================================================================
    MATCH_F(increment_decrement_op) {
      // Match some other comparison ops.
      return any<INCREMENT_DECREMENT_OPS>();
    }

    // ===================================================================================================================
    MATCH_F(other_comparison_op) {
      // Match some other comparison ops.
      return any<OTHER_COMPARISON_OPS>();
    }

    // ===================================================================================================================
    MATCH_F(boolean_op) {
      // Match some other some boolean ops.
      return any<BOOLEAN_OPS>();
    }

#undef X

    // ===================================================================================================================
    MATCH_F(lispesque_keyword) {
      // Match a set of strings that look like reasonable Lisp keyword symbol names.
      constexpr auto HEAD_MF {my all<my character<':'>, my alpha, my star<my alnums>>};
      constexpr auto SEPARATOR_MF {my plus<my character<'-'>>};
      constexpr auto TAIL_MF {my plus<my alnums>};

      return with_lispesque_token_terminator<
        my all<
          HEAD_MF,
          my star<
            my all<
              SEPARATOR_MF,
              TAIL_MF>>,
          my optional<
            my any<
              my character<'!'>,
              my character<'?'>>>>>();
    }

    // ===================================================================================================================
    MATCH_F(lispesque_identifier) {
      // Match a set of strings that look like reasonable Lisp symbol names.
      constexpr auto HEAD_MF {my all<my alpha, my star<my alnums>>};
      // constexpr auto SEPARATOR_MF {&t::any<&t::plus<&t::character<'-'>>>};
      constexpr auto TAIL_MF {my plus<my alnums>};

      return with_lispesque_token_terminator<
        my any<
          my with_lispesque_token_terminator<my basic_math_op>,
          my with_lispesque_token_terminator<my basic_comparison_op>,
          my other_comparison_op,
          my increment_decrement_op,
          my boolean_op,
          my all<
            HEAD_MF,
            my star<
              my all<
                my any<
                  my plus<my character<'-'>>,
                  my plus<my character<':'>>,
                  my plus<my character<'/'>>>,
                TAIL_MF>>,
            my optional<
              my any<
                my character<'!'>,
                my character<'?'>>>>>>();
    }

    // ===================================================================================================================
    MATCH_F(c_style_identifier) {
      // Match unqualified C-style identifiers. This should probably match all of them, I think?
      // If anything, it's too broad: it will accept '__' or '___', etc., I'm not immediately certain if those are
      // legal in #C.... those might be legal, but they're also /weird/. Whatever, we'll accept 'em for now.
      return all<
        my any<
          my character<'_'>,
          my alpha>,
        my plus<
          my any<my character<'_'>,
                 my alnums>>>();
    }

    // ===================================================================================================================
    MATCH_F(ignore_whitespace) {
      // Ignore any number of whitespace characers.
      return ignore<my star_whitespaces>();
    }    

    // ===================================================================================================================
    // Manufacture functions of type match_f corresponding to various C-style string predicate functions.
    // ===================================================================================================================
    FROM_C_CHAR_F(alnum,          isalnum);
    FROM_C_CHAR_F(alpha,          isalpha);
    FROM_C_CHAR_F(digit,          isdigit);
    FROM_C_CHAR_F(xdigit,         isxdigit);
    FROM_C_CHAR_F(whitespace,     iswhitespace);
    FROM_C_CHAR_F(non_whitespace, negate<iswhitespace>);

    // ===================================================================================================================
    // Character predicate helper static functions
    // ===================================================================================================================
    template <int (*fun)(int)>
    constexpr static int negate(int c) {
      // Make a negated version of a C-style character predicate function.
      return 0 == fun(c) ? 1 : 0;
    }
     
    // ===================================================================================================================
  };
  // =====================================================================================================================
}
// =======================================================================================================================

// =======================================================================================================================
// Don't leak the macros!
// =======================================================================================================================
#include "undef_macros.hpp"
// =======================================================================================================================
#endif

