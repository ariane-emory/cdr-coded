#ifndef RESEUNE_C_STR_CURSOR_HPP
#define RESEUNE_C_STR_CURSOR_HPP

// =================================================================================================================
namespace reseune {
  
  // ===============================================================================================================
  class c_str_cursor {
  private:
    const char * const m_begin;
    const char * const m_end;
    const char *       m_position;

  public:
  
    // =============================================================================================================
    constexpr inline c_str_cursor(const char * const str) :
      m_begin(str),
      m_end(&m_begin[strlen(str)]),
      m_position(m_begin) {}

    // =============================================================================================================
    constexpr const char * begin() const {
      return m_begin;
    }

    // =============================================================================================================
    constexpr const char * end() const {
      return m_end;
    }
  
    // =============================================================================================================
    inline char operator ++ () {
      ++m_position;
      return *m_position;
    }

    // =============================================================================================================
    inline char operator ++ (int) {
      char c = *m_position;
      ++m_position;
      return c;
    }
  
    // =============================================================================================================
    inline char operator -- () {
      --m_position;
      return *m_position;
    }

    // =============================================================================================================
    inline char operator -- (int) {
      char c = *m_position;
      --m_position;
      return c;
    }
  
    // =============================================================================================================
    inline void discard_while(bool (*predicate)(const char)) {
      char c;

      do { c = (*this)++; }
      while (is_whitespace(c));
      --*this;
    }

    // =============================================================================================================
    inline char * take_until (bool (*predicate)(const char)) {
      char c;
  
      const char * begin = m_position;
  
      do { c = (*this)++; }
      while (0 != c && !predicate(c));
      --*this;
  
      size_t len  = uintptr(m_position) - uintptr(begin);

      if (0 == len)
        return nullptr;
  
      size_t siz  = (len + 1) * sizeof(char);
      char * word = static_cast<char *>(malloc(siz));

      memcpy(word, begin, siz);

      word[len] = 0;

      return word;
    }

    // =============================================================================================================
    inline void discard_whitespace() {
      discard_while(is_whitespace);
    }

    // =============================================================================================================
    inline char * take_word () {
      return take_until(is_whitespace);
    }

    // =============================================================================================================
  };
  // ===============================================================================================================
}
// =================================================================================================================

#endif
