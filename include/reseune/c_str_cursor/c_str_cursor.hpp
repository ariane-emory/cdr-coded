#ifndef RESEUNE_C_STR_CURSOR_HPP
#define RESEUNE_C_STR_CURSOR_HPP

// =================================================================================================================
namespace reseune {
  
  // ===============================================================================================================
  class c_str_cursor {
  private:
    const char * const m_start;
    const char * const m_end;

  public:
    const char *       position;
  
    // =============================================================================================================
    inline c_str_cursor(const char * const str) :
      m_start(str),
      m_end(&m_start[strlen(str)]),
      position(m_start) {}

    // =============================================================================================================
    const char * begin() const {
      return m_start;
    }

    // =============================================================================================================
    const char * end() const {
      return m_end;
    }
  
    // =============================================================================================================
    inline char operator ++ () {
      ++position;
      return *position;
    }

    // =============================================================================================================
    inline char operator ++ (int) {
      char c = *position;
      ++position;
      return c;
    }
  
    // =============================================================================================================
    inline char operator -- () {
      --position;
      return *position;
    }

    // =============================================================================================================
    inline char operator -- (int) {
      char c = *position;
      --position;
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
  
      const char * begin = position;
  
      do { c = (*this)++; }
      while (0 != c && !predicate(c));
      --*this;
  
      size_t len  = uintptr(position) - uintptr(begin);

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
