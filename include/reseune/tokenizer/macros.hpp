#ifndef RESEUNE_TOKENIZER_MACROS_HPP
#define RESEUNE_TOKENIZER_MACROS_HPP

// ===============================================================================================================
// Macros
// ===============================================================================================================

#define HERE                  (**this)
#define MATH_OPS              X('+'), X('-'), X('/'), X('*'), X('%')      
#define MATCH                 CALL_MATCH_F(MF)
#define MAYBE_RETURN_MATCH    if (match) RETURN_MATCH
#define MAYBE_RETURN_NO_MATCH if (!match) RETURN_NO_MATCH
#define NEXT                  ((*this)++)
#define NO_MATCH              (span{nullptr, nullptr, false})
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

#endif
