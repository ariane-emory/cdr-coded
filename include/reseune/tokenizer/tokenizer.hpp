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
#define DO_MATCH           (MATCH(MF))
#define MATCH(tf)          (this->*tf)()
#define HERE               (**this)
#define MOVED              (begin != m_position)
#define UNMOVED            (! MOVED)
#define REWIND             (m_position = begin)
#define NOT_NULL           (0 != HERE)
#define CHAR_MATCHES       (CF(HERE))
#define T_CHAR_F           template <char_f CF>
#define T_CHAR             template <char C>
#define T_MATCH_F          template <match_f MF>
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
      return ! CF(c);
    }
    
    // =============================================================================================================
    T_MATCH_F MATCH_F(ignore) {
      DO_MATCH;
      NOTHING;
    }

    // =============================================================================================================
    MATCH_F(whitespace) {
      return chars<is_whitespace>();
    }

    // =============================================================================================================
    MATCH_F(ignore_whitespace) {
      return ignore<&t::whitespace>();
    }

    // =============================================================================================================
    MATCH_F(word) {
      return until<is_whitespace>();
    }

    // =============================================================================================================
    T_CHAR_F MATCH_F(until) {
      return chars<negate<CF>>();
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
        DO_MATCH;
        if UNMOVED
          NOTHING;
        MATCH(star<MF>);
        YIELD;
      }

      // =============================================================================================================
      T_MATCH_F MATCH_F(star) {
        BEGIN;
        do {
          DO_MATCH;
        } while MOVED;
        YIELD;
      }

      // =============================================================================================================
      T_CHAR_F MATCH_F(chars) {
        //return star<chr<CF>>();
      
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

#undef BACK
#undef BEGIN
#undef CHAR_MATCHES
#undef DO_MATCH
#undef HERE
#undef MATCH
#undef MATCH_F
#undef MOVED
#undef NEXT
#undef NOTHING
#undef NOT_NULL
#undef REWIND
#undef T_CHAR
#undef T_CHAR_F
#undef T_MATCH_F
#undef UNMOVED
#undef YIELD
#undef unless

#endif
