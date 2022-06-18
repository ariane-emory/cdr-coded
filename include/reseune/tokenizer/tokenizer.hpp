#ifndef RESEUNE_TOKENIZER_HPP
#define RESEUNE_TOKENIZER_HPP

#include "reseune/util/util.hpp"
#include "reseune/c_str_cursor/c_str_cursor.hpp"

#define TOKFUN(name, ...) inline char * name(__VA_ARGS__)

// =================================================================================================================
namespace reseune {
  
  // ===============================================================================================================
  class tokenizer : public c_str_cursor {
  private:
    using charfun_t = bool(*)(const char);
    using tokfun_t = char* (tokenizer::*)();
    using t = tokenizer;

  public:

    struct span {
      const char * const begin;
      const char * const end;
    };
    
    // =============================================================================================================
    constexpr inline tokenizer(const char * const str) : c_str_cursor(str) {}

    // =============================================================================================================
    template <charfun_t predicate>
    inline static bool negate(char c) {
      return !predicate(c);
    }
    
    // =============================================================================================================
    template <tokfun_t tokfun>
    TOKFUN(ignore) {
      return (this->*tokfun)(), nullptr;
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
      char * ret {(this->*left)()};

      return (nullptr == ret
              ? (this->*right)()
              : ret);
    }

    // =============================================================================================================
    template <charfun_t predicate>
    TOKFUN(one) {
      if (negate<predicate>(*m_position))
        return nullptr;

      const char * begin {m_position};
      ++m_position;

      return create_new_c_str(span{begin, m_position});
    }

    // =============================================================================================================
    template <tokfun_t tokfun>
    TOKFUN(plus) {
      const char * const begin {m_position};

      (this->*tokfun)();

      return nullptr;
    }

      // =============================================================================================================
      template <charfun_t predicate>
        TOKFUN(star) {
        char c;
  
        const char * const begin {m_position};
  
        do { c = (*this)++; }
        while (0 != c && predicate(c));
        --*this;

        return create_new_c_str(span{begin, m_position});
    }

  private: 

    // =============================================================================================================
    static inline char * create_new_c_str(span const & tok) {      
      const size_t len  {uintptr(tok.end) - uintptr(tok.begin)};
      
      if (0 == len) return nullptr;
  
      const size_t siz  {(len + 1) * sizeof(char)};
      char *       word {static_cast<char *>(malloc(siz))};

      memcpy(word, tok.begin, siz);

      word[len] = 0;

      return word;
    }

    // =============================================================================================================
  };
  // ===============================================================================================================
}
// =================================================================================================================

#undef TOKFUN

#endif
