#ifndef RESEUNE_TOKENIZER_HPP
#define RESEUNE_TOKENIZER_HPP

#include <tuple>

#include "reseune/util/util.hpp"
#include "reseune/c_str_cursor/c_str_cursor.hpp"

#define MATCH_F(name, ...) inline span name(__VA_ARGS__)
#define BEGIN              char c; std::ignore = c; const char * const begin {m_position}
#define NOTHING            return span{}
#define BACK               (--*this)
#define NEXT               (c = ((*this)++))
#define YIELD              return span{begin, m_position}
#define DO_MATCH           (MATCH(tokfun))
#define MATCH(tf)          (this->*tf)()
#define HERE               (**this)
#define MOVED              (begin != m_position)
#define UNMOVED            (! MOVED)
#define REWIND             (m_position = begin)
#define NOT_NULL           (0 != HERE)
#define CHAR_MATCHES       (predicate(HERE))
#define T_CHAR_F           template <char_f predicate>
#define T_CHAR             template <char C>
#define T_MATCH_F          template <match_f tokfun>
#define unless(expr)       if (! (expr))

// =================================================================================================================
namespace reseune {
  
  // ===============================================================================================================
  class tokenizer : public c_str_cursor {
  public:
    struct span {
      const char * begin;
      const char * end;

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
    using char_f = bool (*)(const char);
    using match_f = span (tokenizer::*)();
    using t = tokenizer;

  public:
    
    // =============================================================================================================
    constexpr inline tokenizer(const char * const str) : c_str_cursor(str) {}

    // =============================================================================================================
    T_CHAR_F inline static bool negate(char c) {
      return ! predicate(c);
    }
    
    // =============================================================================================================
    T_MATCH_F MATCH_F(ignore) {
      DO_MATCH;
      NOTHING;
    }

    // =============================================================================================================
    MATCH_F(ignore_whitespace) {
      return ignore<&t::chars<is_whitespace>>();
    }

    // =============================================================================================================
    MATCH_F(word) {
      return until<is_whitespace>();
    }

    // =============================================================================================================
    T_CHAR_F MATCH_F(until) {
      return chars<negate<predicate>>();
    }

    // =============================================================================================================
    template <match_f left, match_f right>
    MATCH_F(either) {
      BEGIN;
      MATCH (left);
      if MOVED
        YIELD;
      MATCH (right);
      YIELD;
    }


    // =============================================================================================================
    T_CHAR MATCH_F(chr) {
      return chr<is_char<C>>();
    }
    
    // =============================================================================================================
    T_CHAR_F MATCH_F(chr) {
      BEGIN;      
      unless (CHAR_MATCHES)
        NOTHING;
      NEXT;      
      YIELD;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(plus) {
      BEGIN;
      MATCH(tokfun);
      if UNMOVED
        NOTHING;
      MATCH(chars<tokfun>);
      YIELD;
    }

    // =============================================================================================================
    T_CHAR_F MATCH_F(chars) { 
      BEGIN;
      while (NOT_NULL && CHAR_MATCHES)
        NEXT;      
      YIELD;
    }

  private: 

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
#undef BEGIN
#undef NOTHING
#undef BACK
#undef NEXT
#undef YIELD
#undef MATCH
#undef HERE
#undef MOVED
#undef UNMOVED
#undef REWIND
#undef NOT_NULL
#undef CHAR_MATCHES
#undef T_CHAR_F
#undef T_MATCH_F
#undef unless

#endif
