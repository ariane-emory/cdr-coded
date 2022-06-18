#ifndef RESEUNE_TOKENIZER_HPP
#define RESEUNE_TOKENIZER_HPP

#include "reseune/c_str_cursor/c_str_cursor.hpp"

// =================================================================================================================
namespace reseune {
  
  // ===============================================================================================================
  class tokenizer : public c_str_cursor {
  private:
    using predicate_t = bool(*)(const char);
    using tokfun_t = char* (tokenizer::*)();
    
  public:
  
    // =============================================================================================================
    constexpr inline tokenizer(const char * const str) : c_str_cursor(str) {}

    // =============================================================================================================
    template <tokfun_t left, tokfun_t right>
    inline char * either() {
      char * ret {(this->*left)()};
      return (nullptr == ret
              ? (this->*right)()
              : ret);
    }

    // =============================================================================================================
    template <predicate_t p>
    inline void ignore_while() {
      char c;

      do { c = (*this)++; }
      while (is_whitespace(c));
      --*this;
    }

    // =============================================================================================================
    template <predicate_t p>
    inline char * one () {
      const char * begin = m_position;

      if (!p(*m_position)) return nullptr;
      
      (*this)++;
      
      return create_new_c_str(begin, m_position);
    }

    // =============================================================================================================
    template <predicate_t p>
    inline char * until() { return until(p); }
    inline char * until(predicate_t predicate) {
      char c;
  
      const char * begin = m_position;
  
      do { c = (*this)++; }
      while (0 != c && !predicate(c));
      --*this;

      return create_new_c_str(begin, m_position);
    }

    // =============================================================================================================
    template <predicate_t p>
    inline char * star() { return star(p); }
    inline char * star(predicate_t predicate) {
      char c;
  
      const char * begin = m_position;
  
      do { c = (*this)++; }
      while (0 != c && predicate(c));
      --*this;

      return create_new_c_str(begin, m_position);
    }

    // =============================================================================================================
    inline void ignore_whitespace() {
      star<is_whitespace>();
    }

    // =============================================================================================================
    inline char * word () {
      return until<is_whitespace>();
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

#endif
