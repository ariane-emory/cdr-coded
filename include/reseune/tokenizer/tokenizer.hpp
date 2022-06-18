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
    
    // =============================================================================================================
    constexpr inline tokenizer(const char * const str) : c_str_cursor(str) {}

    // =============================================================================================================
    template <charfun_t predicate>
    inline static bool negate(char c) {
      return !predicate(c);
    }
    
    // =============================================================================================================
    template <tokfun_t token>
    TOKFUN(ignore) {
      (this->*token)();

      return nullptr;
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
      const char * begin = m_position;

      if (!predicate(*m_position))
        return nullptr;

      (*this)++;      

      return create_new_c_str(begin, m_position);
    }

    // =============================================================================================================
    template <charfun_t predicate>
    TOKFUN(star) {
      char c;
  
      const char * begin = m_position;
  
      do { c = (*this)++; }
      while (0 != c && predicate(c));
      --*this;

      return create_new_c_str(begin, m_position);
    }

  private: 

    // =============================================================================================================
    inline char * create_new_c_str(const char * begin, const char * end) {      
      size_t len  = uintptr(m_position) - uintptr(begin);
      
      if (0 == len) return nullptr;
  
      size_t siz  = (len + 1) * sizeof(char);
      char * word = static_cast<char *>(malloc(siz));

      memcpy(word, begin, siz);

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
