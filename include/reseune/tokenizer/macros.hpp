#ifndef RESEUNE_TOKENIZER_MACROS_HPP
#define RESEUNE_TOKENIZER_MACROS_HPP

// =====================================================================================================================
// Macros
// =====================================================================================================================

#define primitive_symbols                                                       \
  X('t'),                                                                       \
    X('i','f'),                                                                 \
    X('o','r'),                                                                 \
    X('a','n','d'),                                                             \
    X('c','a','r'),                                                             \
    X('c','d','r'),                                                             \
    X('l','e','t'),                                                             \
    X('n','i','l'),                                                             \
    X('n','o','t'),                                                             \
    X('c','o','n','s'),                                                         \
    X('e','q','l','?'),                                                         \
    X('e','x','i','t'),                                                         \
    X('l','i','s','t'),                                                         \
    X('n','i','l','?'),                                                         \
    X('s','e','t','!'),                                                         \
    X('a','t','o','m','?'),                                                     \
    X('d','e','f','i','n','e'),                                                 \
    X('l','a','m','b','d','a'),                                                 \
    X('l','i','s','t','?'),                                                     \
    X('p','r','i','n','t'),                                                     \
    X('r','e','t','u','r','n')
#define increment_decrement_ops                                                 \
  X('+', '+'),                                                                  \
    X('-', '-'),                                                                \
    X('+', '='),                                                                \
    X('-', '='),                                                                \
    X('*', '='),                                                                \
    X('/', '='),                                                                \
    X('&', '='),                                                                \
    X('|', '='),                                                                \
    X('~', '=') 
#define other_comparison_ops                                                    \
  X('!', '='),                                                                  \
    X('=', '='),                                                                \
    X('<', '='),                                                                \
    X('>', '=')                                                          
#define boolean_ops                                                             \
  X('&', '&'),                                                                  \
    X('|', '|')
#define primitive_math_ops        X('+'), X('-'), X('/'), X('*')
#define other_math_ops        X('%'), X('~'), X('^'), X('&')
#define primitive_comparison_ops  X('!'), X('='), X('>'), X('<')

#define EMPTY                 (span_type{POS, POS})
#define HERE                  (*cursor)
#define MATCH                 CALL_MATCH_F(MF)
#define MAYBE_RETURN_MATCH    if (match) RETURN_MATCH
#define MAYBE_RETURN_NO_MATCH if (!match) RETURN_NO_MATCH
#define my                    &t::
#define NEXT                  ((cursor)++)
#define NO_MATCH              (span_type{nullptr, nullptr, false})
#define NUL_HERE              (0 == HERE)
#define POS                   (cursor.pos())
#define REWIND                (cursor.set_pos(start))
#define SPAN                  span_type{start, POS}
#define START                 log("%s: Enter.", __FUNCTION__); MARK(start); span_type match{NO_MATCH};
#define RETURN_MATCH          {log("%s: Returning match after moving %zu.", __FUNCTION__, POS - start);  return match;}
#define RETURN_NO_MATCH       {log("%s: Returning no_match after moving %zu.", __FUNCTION__, POS - start); return NO_MATCH;}
#define RETURN_SPAN           {log("%s: Returning span after moving %zu.", __FUNCTION__, POS - start); return SPAN;}
#define RETURN_EMPTY          {log("%s: Returning empty after moving %zu.", __FUNCTION__, POS - start); return EMPTY;}

#define T_MATCH_F             template <match_f MF>

#define CALL_MATCH_F(match_f) {indentation += 2; match = (this->*match_f)(); indentation -= 2;}
#define MARK(name)            const char * const name{POS}; std::ignore = name
#define unless(expr)          if (! (expr))
#define until(expr)           while (! (expr))

#define CHAR_F(name)          constexpr inline static bool name(const char c)
#define MATCH_F(name, ...)    constexpr inline span_type name(__VA_ARGS__)
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

// =====================================================================================================================
#endif
