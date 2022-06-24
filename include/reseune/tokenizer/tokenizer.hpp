#ifndef RESEUNE_TOKENIZER_HPP
#define RESEUNE_TOKENIZER_HPP

#include <tuple>
#include <cassert>

#include "../util/util.hpp"
#include "../c_str_cursor/c_str_cursor.hpp"
#include "tokenizer_span.hpp"
#include "tokenizer_span_with_children.hpp"
#include "lispesque_token_types.hpp"

#include "macros.hpp" // include last!

// =====================================================================================================================
namespace reseune {
  // ===================================================================================================================
  // tokenizer abstract class
  // ===================================================================================================================
  template <template <typename> typename T_SPAN_T, typename LABEL_T>
  struct tokenizer {

    // =================================================================================================================
    // Constructors
    // =================================================================================================================
    constexpr tokenizer(const char * const str) : cursor(str), verbose(false), indentation(0) {}
    
    // =================================================================================================================
    // Public types
    // =================================================================================================================
    using span_type  = T_SPAN_T<LABEL_T>;
    using label_type = LABEL_T;

  protected:
    c_str_cursor cursor;
    
    // These are both only used by the log method below for debug output:
    bool         verbose;
    size_t       indentation;

    // =================================================================================================================
    // Debug logging helper functions
    // =================================================================================================================

    constexpr void log(const char * format, ...) const {
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
    // Protected types
    // =================================================================================================================
    using char_f     = int (*)(int);
    using match_f    = span_type (tokenizer::*)();
    using t          = tokenizer;

  public:
    
    // ===================================================================================================================
    // Member functions
    // =================================================================================================================
    t & operator >> (span_type & span) {
      // Extract the next token.
      span = token();
      return *this;
    }

  protected:
    
    // =================================================================================================================
    // Pure virtual member functions
    // =================================================================================================================
    virtual MATCH_F(token) = 0;

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

    // =================================================================================================================
    T_MATCH_F MATCH_F(plus) {
      // Match against MF one or more times.
      START;
      constexpr auto mf {my all<MF, my star<MF>>};
      CALL_MATCH_F(mf);
      RETURN_MATCH;
    }

    // =================================================================================================================
    T_MATCH_F MATCH_F(zero_padded) {
      // Ignore any number of 0s and then match against MF. This needs a non-greedy version.
      ignore<my star<my character<'0'>>>();
      START;
      MATCH;
      MAYBE_RETURN_MATCH;
      REWIND;
      RETURN_NO_MATCH;
    }

    // =================================================================================================================
    T_MATCH_F MATCH_F(optional) {
      // Match against MF and if it returns NO_MATCH, return an empty span instead.
      START;
      MATCH;
      MAYBE_RETURN_MATCH;
      RETURN_EMPTY;
    }

    // =================================================================================================================
    T_MATCH_F MATCH_F(followed_by) {
      // Match against MF, rewind, and if it returned a match, return empty.
      START;
      MATCH;
      REWIND;
      MAYBE_RETURN_NO_MATCH;
      RETURN_EMPTY;
    }

    // =================================================================================================================
    T_MATCH_F MATCH_F(not_followed_by) {
      // Match against MF, rewined, and if it returned a match, return NO_MATCH, otherwise return empty.
      return followed_by<my negate<MF>>();
    }

    // =================================================================================================================
    T_MATCH_F MATCH_F(negate) {
      // Match against MF and if it returns a match, and return NO_MATCH, otherwise return empty.
      START;
      MATCH;
      unless (match)
        RETURN_EMPTY;
      RETURN_NO_MATCH;
    }

    // =================================================================================================================
    T_MATCH_F MATCH_F(ignore) {
      // Match against MF and ignore the result by returning empty.
      // Should this ever return NO_MATCH? Maybe...
      START;
      MATCH;
      RETURN_EMPTY;
    }

    // =================================================================================================================
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

    // =================================================================================================================
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

    // =================================================================================================================
    template<label_type L, match_f MF>
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

    // =================================================================================================================
    template <match_f MF, match_f... MFs>
    MATCH_F(all) {
      // Match MF followed by the MFs in order.
      START;
      MATCH;
      MAYBE_RETURN_NO_MATCH;
      constexpr auto TAIL_MF {my all<MFs...>};
      CALL_MATCH_F(TAIL_MF);
      if (match)
        RETURN_SPAN;
      REWIND;
      RETURN_NO_MATCH;
    }
    
    template <typename... nil>
    MATCH_F(all) {
      return EMPTY;
    }

    // =================================================================================================================
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
      return NO_MATCH;
    }

    // =================================================================================================================
    template <char C, char... Cs>
    MATCH_F(character) {
      // Match either the character C or one of the Cs.
      START;
      log("Compare '%c' (%u) with '%c' (%u).", HERE, HERE, C, C);
      constexpr auto mf {my character_f<ischar<C>>};
      CALL_MATCH_F(mf);
      MAYBE_RETURN_MATCH;
      return character<Cs...>();
    }

    template <typename... nil>
    MATCH_F(character) {
      return NO_MATCH;
    }

    // =================================================================================================================
    template <char C, char... Cs>
    MATCH_F(word) {
      // Match C followed by all the Cs in order.
      START;
      constexpr auto MF {my character<C>};
      MATCH;
      MAYBE_RETURN_NO_MATCH;
      constexpr auto TAIL_MF {my word<Cs...>};
      CALL_MATCH_F(TAIL_MF);
      if (match)
        RETURN_SPAN;
      REWIND;
      RETURN_NO_MATCH;
    }
    
    template <typename... nil>
    MATCH_F(word) {
      return EMPTY;
    }
    
    // =================================================================================================================
    template <char C>
    MATCH_F(characters) {
      // Match either one or more Cs;
      START;
      constexpr auto mf {my plus<my character_f<ischar<C>>>};
      CALL_MATCH_F(mf);
      RETURN_MATCH;
    }
    
    // =================================================================================================================
    template <match_f HEAD_MF, match_f SEPARATOR_MF, match_f TAIL_MF>
    MATCH_F(intercalate) {
      // Match HEAD_MF followed by any number of TAIL_MFs separated by SEPARATOR_MFs.
      return all<
        HEAD_MF,
        my star<
          my all<
            SEPARATOR_MF,
            TAIL_MF>>>();
    }

    // =================================================================================================================
    // Convenience match functions
    // =================================================================================================================
    MATCH_F(do_not_match) {
      // Always return NO_MATCH
      return NO_MATCH;
    }
    
    // =================================================================================================================
    MATCH_F(positive_integer) {
      // Match a positive integer. Does not permit a leading '+'!
      return any<
        my zero_padded<my digits>,
        my plus<my character<'0'>>>();
    }
    
    // =================================================================================================================
    MATCH_F(integer) {
      // Match any integer (with or without leading zeroes).
      return all<
        my optional<my character<'-', '+'>>,
        my digits>();
    }

    // =================================================================================================================
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

    // =================================================================================================================
    MATCH_F(ignore_whitespace) {
      // Ignore any number of whitespace characers.
      return ignore<my star_whitespaces>();
    }    

    // =================================================================================================================
    // Manufacture functions of type match_f corresponding to various C-style string predicate functions. Each macro
    // invocation makes 3 match_f functions: name. names and star_name.
    // =================================================================================================================
    FROM_C_CHAR_F(alnum,          isalnum);
    FROM_C_CHAR_F(alpha,          isalpha);
    FROM_C_CHAR_F(digit,          isdigit);
    FROM_C_CHAR_F(xdigit,         isxdigit);
    FROM_C_CHAR_F(whitespace,     iswhitespace);
    FROM_C_CHAR_F(non_whitespace, negate<iswhitespace>);

    // =================================================================================================================
    // Character predicate helper static functions
    // =================================================================================================================
    template <int (*fun)(int)>
    constexpr static int negate(int c) {
      // Make a negated version of a C-style character predicate function.
      return 0 == fun(c) ? 1 : 0;
    }
     
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

