#ifndef RESEUNE_TOKENIZER_HPP
#define RESEUNE_TOKENIZER_HPP

#include <tuple>

#include "reseune/util/util.hpp"
#include "reseune/c_str_cursor/c_str_cursor.hpp"

#define BACK               (--*this)
#define CHAR_MATCHES       (CF(HERE))
#define DO_MATCH(match_f)  span match_f ## _match {(this->*match_f)()}
#define HERE               (**this)
#define MARK(name)         const char * const name{POS}; std::ignore = name
#define MATCH              span match {(this->*MF)()}
#define MATCH_F(name, ...) inline span name(__VA_ARGS__)
#define MOVED              (start != POS)
#define NEXT               (c = ((*this)++))
#define NOTHING            return span{}
#define NOT_NULL           (0 != HERE)
#define POS                (m_position)         
#define REWIND(name)       (POS = name)
#define SPAN               span{start, POS}
#define START              char c; std::ignore = c; MARK(start)
#define STASH              span stashed{match}
#define T_CHAR             template <char C>
#define T_CHAR_F           template <char_f CF>
#define T_MATCH_F          template <match_f MF>
#define UNMOVED            (! MOVED)
#define UNSTASH            return stashed
#define YIELD              return SPAN
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
    constexpr inline tokenizer(const char * const str) : c_str_cursor(str) {}

    // =============================================================================================================
    // Character predicate functions
    // =============================================================================================================
    T_CHAR_F inline static bool negate(const char c) {
      // Make a negated version of a character predicate function.      
        return ! CF(c);
    }
     
    // =============================================================================================================
    // Match functions
    // =============================================================================================================
    T_CHAR_F MATCH_F(character_f) {
      // Make a match_f from a char predicate function.
      START;      
      unless (NOT_NULL & CHAR_MATCHES)
        NOTHING;
      NEXT;      
      YIELD;
    }


    // =============================================================================================================
    T_CHAR MATCH_F(character) {
      // Make a match_f for a particular character.
      return character_f<is_char<C>>();
    }

    // =============================================================================================================
    
    T_MATCH_F MATCH_F(ignore) {
      // Match a match_f and ignore the result.
      MATCH;
      NOTHING;
    }

    // =============================================================================================================
    template<LABEL_T l, match_f MF> MATCH_F(label) {
      // Label the token type of a match_f
      START;
      MATCH;
      STASH;
      unless (stashed.empty())
        stashed.label = l;
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
    T_MATCH_F MATCH_F(star) {
      START;

      const char * last_pos;
      
      do {
        last_pos = POS;
        MATCH;
      } while (NOT_NULL && POS != last_pos);
      
      YIELD;
    }

    // =============================================================================================================
    // Convenience match functions
    // =============================================================================================================
    MATCH_F(whitespace) {
      return star<&t::character_f<is_whitespace>>();
    }

    // =============================================================================================================
    MATCH_F(non_whitespace) {
      return star<&t::character_f<negate<is_whitespace>>>();
    }

    // =============================================================================================================
    MATCH_F(word) {
      return plus<&t::non_whitespace>();
    }

    // =============================================================================================================
    MATCH_F(ignore_whitespace) {
      return ignore<&t::whitespace>();
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

#define MATCH_F(name, ...) inline span name(__VA_ARGS__)
#define START              char c; std::ignore = c; MARK(start)
#define NOTHING            return span{}
#define BACK               (--*this)
#define NEXT               (c = ((*this)++))
#define YIELD              return SPAN
#define SPAN               span{start, POS}
#define STASH              span stashed{match}
#define UNSTASH            return stashed
#define MATCH              span match {(this->*MF)()}
#define DO_MATCH(match_f)  span match_f ## _match {(this->*match_f)()}
#define HERE               (**this)
#define MOVED              (start != POS)
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

#endif
