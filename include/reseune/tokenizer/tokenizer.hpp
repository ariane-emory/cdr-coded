#ifndef RESEUNE_TOKENIZER_HPP
#define RESEUNE_TOKENIZER_HPP

#include "reseune/c_str_cursor/c_str_cursor.hpp"

// =================================================================================================================
namespace reseune {
  
  // ===============================================================================================================
  class tokenizer : public c_str_cursor {
  private:
    using predicate_t = bool(*)(const char);
    using tokfun_t = char * (*)();
    
  public:
  
    // =============================================================================================================
    constexpr inline tokenizer(const char * const str) : c_str_cursor(str) {}

    // =============================================================================================================
    template <tokfun_t t1, tokfun_t t2>
    inline char * either() { 
      char * ret {t1()};
      return (nullptr == ret
              ? t2()
              : ret);
    }

    // =============================================================================================================
    template <predicate_t p>
    inline void discard_while() {
      discard_while(p);
    }
    inline void discard_while(predicate_t predicate) {
      char c;

      do { c = (*this)++; }
      while (is_whitespace(c));
      --*this;
    }

    // =============================================================================================================
    inline char * take_one (predicate_t predicate) {
      const char * begin = m_position;

      if (!predicate(*m_position)) return nullptr;
      
      (*this)++;
      
      return create_new_c_str(begin, m_position);
    }

    // =============================================================================================================
    template <predicate_t p>
    inline char * take_until() {
      return take_until(p);
    }
    inline char * take_until(predicate_t predicate) {
      char c;
  
      const char * begin = m_position;
  
      do { c = (*this)++; }
      while (0 != c && !predicate(c));
      --*this;

      return create_new_c_str(begin, m_position);
    }

    // =============================================================================================================
    template <predicate_t p>
    inline char * take_while() {
      return take_while(p);
    }
    inline char * take_while(predicate_t predicate) {
      char c;
  
      const char * begin = m_position;
  
      do { c = (*this)++; }
      while (0 != c && predicate(c));
      --*this;

      return create_new_c_str(begin, m_position);
    }

    // =============================================================================================================
    inline void discard_whitespace() {
      discard_while<is_whitespace>();
    }

    // =============================================================================================================
    inline char * take_word () {
      return take_until<is_whitespace>();
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
