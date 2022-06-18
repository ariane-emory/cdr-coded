#ifndef RESEUNE_TOKENIZER_HPP
#define RESEUNE_TOKENIZER_HPP

#include <tuple>

#include "reseune/util/util.hpp"
#include "reseune/c_str_cursor/c_str_cursor.hpp"

#define TOKFUN(name, ...) inline span name(__VA_ARGS__)
#define BEGIN             char c; std::ignore = c; const char * const begin {m_position}
#define NOTHING           return span{}
#define BACK              (--*this)
#define CHOMP             (c = ((*this)++))
#define YIELD             return span{begin, m_position}
#define MATCH(tf)          (this->*tf)()
#define HERE              (**this)
#define MOVED             (begin != m_position)
#define UNMOVED           (! MOVED)
#define REWIND            m_position = begin

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
    using charfun_t = bool (*)(const char);
    using tokfun_t = span (tokenizer::*)();
    using t = tokenizer;

  public:
    
    // =============================================================================================================
    constexpr inline tokenizer(const char * const str) : c_str_cursor(str) {}

    // =============================================================================================================
    template <charfun_t predicate>
    inline static bool negate(char c) {
      return ! predicate(c);
    }
    
    // =============================================================================================================
    template <tokfun_t tokfun>
    TOKFUN(ignore) {
      MATCH(tokfun);
      NOTHING;
    }

    // =============================================================================================================
    TOKFUN(ignore_whitespace) {
      return ignore<&t::star<is_whitespace>>();
    }

    // =============================================================================================================
    TOKFUN(word) {
      return until<is_whitespace>();
    }

    // =============================================================================================================
    template <charfun_t predicate>
    TOKFUN(until) {
      return star<negate<predicate>>();
    }

    // =============================================================================================================
    template <tokfun_t left, tokfun_t right>
    TOKFUN(either) {
      BEGIN;
      MATCH(left);
      if (MOVED)
        YIELD;
      MATCH(right);
      YIELD;
    }

    // =============================================================================================================
    template <charfun_t predicate>
    TOKFUN(one) {
      BEGIN;      
      if (negate<predicate>(HERE))
        NOTHING;
      CHOMP;      
      YIELD;
    }

    // =============================================================================================================
    template <tokfun_t tokfun>
    TOKFUN(plus) {
      BEGIN;
      MATCH(tokfun);
      if (UNMOVED)
        NOTHING;
      MATCH(star<tokfun>);
      YIELD;
    }

    // =============================================================================================================
    template <charfun_t predicate>
    TOKFUN(star) {        
      BEGIN;
        
      do { CHOMP; }
      while (0 != c && predicate(c));
      BACK;

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

#undef TOKFUN
#undef BEGIN
#undef NOTHING
#undef BACK
#undef CHOMP
#undef YIELD
#undef MATCH
#undef HERE
#undef MOVED
#undef REWIND

#endif
