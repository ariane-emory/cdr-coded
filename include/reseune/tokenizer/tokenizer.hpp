#ifndef RESEUNE_TOKENIZER_HPP
#define RESEUNE_TOKENIZER_HPP

#include <tuple>

#include "reseune/util/util.hpp"
#include "reseune/c_str_cursor/c_str_cursor.hpp"

#define BACK               (--*this)
#define CHAR_MATCHES       (CF(HERE))
#define CHAR_F(name)       inline static bool name(const char c)
#define DO_MATCH(match_f)  match = {(this->*match_f)()}
#define HERE               (**this)
#define MARK(name)         const char * const name{POS}; std::ignore = name
#define MATCH              match = {(this->*MF)()}
#define MATCH_F(name, ...) inline span name(__VA_ARGS__)
#define MOVED              (start != POS)
#define NEXT               ((*this)++)
#define NOTHING            (span{})
#define NOT_NULL           (0 != HERE)
#define POS                (m_position)         
#define REWIND(name)       (POS = name)
#define SPAN               span{start, POS}
#define START              MARK(start); span match {NOTHING}
#define STASH              span stashed{match}
#define T_CHAR             template <char C>
#define T_CHAR_F           template <char_f CF>
#define T_MATCH_F          template <match_f MF>
#define unless(expr)       if (! (expr))

// =================================================================================================================
namespace reseune {
  
  // ===============================================================================================================
  template <typename LABEL_T>
  class tokenizer : public c_str_cursor {
  public:
    struct span {
      const char * begin;
      const char * end;
      LABEL_T label;

      span(
        const char * b = nullptr,
        const char * e = nullptr,
        LABEL_T l = static_cast<LABEL_T>(0)) : begin(b), end(e), label(l) {}
      size_t length() const {
        return end - begin;
      }

      bool empty() const {
        return 0 == length();
      }

      char * c_str() const {
        return create_new_c_str(*this);
      }
    };

  private:

    // ===============================================================================================================
    using char_f  = bool (*)(const char);
    using match_f = span (tokenizer::*)();
    using t       = tokenizer;

  public:
    
    // =============================================================================================================
    // Constructors
    // =========================================================================================================
    constexpr inline tokenizer(const char * const str) : c_str_cursor(str) {}

    // =============================================================================================================
    // Character predicate functions
    // =============================================================================================================
    template <int (*fun)(int c)>
    static bool boolified(char c) {
      // Make a negated version of a character predicate function.
      return 0 != fun(c);
    }
     
    // =============================================================================================================
      T_CHAR_F CHAR_F(negate) {
        // Make a negated version of a character predicate function.
        return ! CF(c);
      }
     
    // =============================================================================================================
    template <char_f left, char_f right>
    CHAR_F(disjoin) {
      // Make a char_f for the disjunction of left and right.
      return left(c) || right(c);
    }
     
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
    template<LABEL_T l, match_f MF> MATCH_F(label) {
      // Match against match_𝑓 and label the token type.
      START;
      MATCH;
      if MOVED
        match.label = l;
      return match;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(strip) {
      // Match against match_f while ignoring any surrounding whitespace (before and after).
      ignore_whitespace();
      START;
      MATCH;
      ignore_whitespace();
      return match;
    }

    // =============================================================================================================
    template <match_f left, match_f right>
    MATCH_F(either) {
      // Match either left and, if fit did not match, match against right.
      START;
      DO_MATCH(left);
      if MOVED
        return match;
      DO_MATCH(right);
      return match;
    }

    template <match_f left, match_f right>
    MATCH_F(both) {
      // Match against left and, if it matched, match against right.
      START;
      DO_MATCH(left);
      unless (MOVED)
        return NOTHING;
      DO_MATCH(right); 
      return SPAN;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(star) {
      // Match against match_f zero or more times.
      START;
      const char * last_pos;
      do {
        last_pos = POS;
        MATCH;
      } while (NOT_NULL && POS != last_pos);
      return SPAN;
    }

    // =============================================================================================================
    // =============================================================================================================
    T_MATCH_F MATCH_F(plus) {
      // Match against match_f one or more times
      return both<MF, &t::star<MF>>();
    }

    // =============================================================================================================
    T_CHAR_F MATCH_F(character_f) {
      // Match a char predicate function.
      START;      
      unless (NOT_NULL & CHAR_MATCHES)
        return NOTHING;
      NEXT;      
      return SPAN;
    }

    // =============================================================================================================
    T_CHAR MATCH_F(character) {
      // Match a particular character.
      return character_f<is_char<C>>();
    }

    // =============================================================================================================
    // Convenience match functions
    // =============================================================================================================
    MATCH_F(whitespace) {
      return star<&t::character_f<is_whitespace>>();
    }

    // =============================================================================================================
    MATCH_F(ignore_whitespace) {
      return ignore<&t::whitespace>();
    }

    // =============================================================================================================
    MATCH_F(non_whitespace) {
      // Match one or more non-white characters.
      return plus<&t::character_f<negate<is_whitespace>>>();
    }

    // =============================================================================================================
    MATCH_F(alnum) {
      // Match one alphanumeric character.
      return character_f<boolified<isalnum>>();
    }

    // =============================================================================================================
    MATCH_F(alpha) {
      // Match onme alphabetic character.
      return character_f<boolified<isalpha>>();
    }

    // =============================================================================================================
    MATCH_F(plain_symbol) {
      return both<&t::alpha, &t::star<&t::alnum>>();
    }

  private: 

    // =============================================================================================================
    // Static functions
    // =============================================================================================================
    static inline char * create_new_c_str(span const & tok) {      
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

#undef MATCH_F
#undef BACK
#undef CHAR_MATCHES
#undef DO_MATCH
#undef HERE
#undef MARK
#undef MATCH
#undef MOVED
#undef NEXT
#undef NOTHING
#undef NOT_NULL
#undef POS
#undef REWIND
#undef SPAN
#undef START
#undef STASH
#undef T_CHAR
#undef T_CHAR_F
#undef T_MATCH_F
#undef unless

#endif
