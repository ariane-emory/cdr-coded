#ifndef RESEUNE_TOKENIZER_HPP
#define RESEUNE_TOKENIZER_HPP

#include <tuple>

#include "reseune/util/util.hpp"
#include "reseune/c_str_cursor/c_str_cursor.hpp"

#define MATCH_F(name, ...) inline span name(__VA_ARGS__)
#define START              char c; std::ignore = c; MARK(begin)
#define NOTHING            return span{}
#define BACK               (--*this)
#define NEXT               (c = ((*this)++))
#define YIELD              return SPAN
#define SPAN               span{begin, POS}
#define STASH              span stashed{match}
#define UNSTASH            return stashed
#define MATCH              span match {(this->*MF)()}
#define DO_MATCH(match_f)  span match_f ## _match {(this->*match_f)()}
#define HERE               (**this)
#define MOVED              (begin != POS)
#define POS                (m_position)         
#define UNMOVED            (! MOVED)
#define REWIND(name)       (POS = name)
#define NOT_NULL           (0 != HERE)
#define CHAR_MATCHES       (CF(HERE))
#define T_CHAR_F           template <char_f CF>
#define T_CHAR             template <char C>
#define T_MATCH_F          template <match_f MF>
#define unless(expr)       if (! (expr))
#define MARK(name)         const char * const name{POS}; std::ignore = name

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

      // span & operator = (const span & other) {
      //   begin = other.begin;
      //   label = other.end;
      //   end   = other.label;
      // }
      
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
    T_MATCH_F MATCH_F(ignore) {
      MATCH;
      NOTHING;
    }

    // =============================================================================================================
    MATCH_F(whitespace) {
      return star<&t::c_f<is_whitespace>>();
    }

    // =============================================================================================================
    MATCH_F(non_whitespace) {
      return star<&t::c_f<negate<is_whitespace>>>();
    }

    // =============================================================================================================
    MATCH_F(word) {
      return plus<&t::non_whitespace>();
    }

    // =============================================================================================================
    MATCH_F(ignore_whitespace) {
      return ignore<&t::whitespace>();
    }

    // =============================================================================================================
    template<LABEL_T l, match_f MF> MATCH_F(label) {
      START;
      MATCH;
      STASH;
      stashed.label = l;
      // printf("stashed(%zu, %zu, %u).\n", stashed.begin, stashed.end, stashed.label);
      UNSTASH;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(strip) {
      ignore_whitespace();
      START;
      MATCH;
      STASH;
      ignore_whitespace();
      UNSTASH;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(plus) {
      START;
      MATCH;
      if UNMOVED
        NOTHING;
      YIELD;
    }

    // =============================================================================================================
    template <match_f left, match_f right>
    MATCH_F(either) {
      START;
      DO_MATCH (left);
      if MOVED
        return left_match;
      DO_MATCH (right);
      return right_match;
    }

    // =============================================================================================================
    T_CHAR_F inline static bool negate(const char c) {
      return ! CF(c);
    }
    
    // =============================================================================================================
    T_CHAR MATCH_F(c) {
      return c_f<is_char<C>>();
    }
    
    // =============================================================================================================
    T_CHAR_F MATCH_F(c_f) {
      START;      
      unless (NOT_NULL & CHAR_MATCHES)
        NOTHING;
      NEXT;      
      YIELD;
    }

    // =============================================================================================================
    T_MATCH_F MATCH_F(star) {
      START;

      const char * last_pos;
      
      do {
        last_pos = POS;
        MATCH;
      } while (NOT_NULL && POS != last_pos);
      
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
#undef START
#undef CHAR_MATCHES
#undef MATCH
#undef HERE
#undef DO_MATCH
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
