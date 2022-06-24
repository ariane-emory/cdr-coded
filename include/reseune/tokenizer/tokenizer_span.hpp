#ifndef RESEUNE_TOKENIZER_SPAN_HPP
#define RESEUNE_TOKENIZER_SPAN_HPP

// =====================================================================================================================
namespace reseune {
  // ===================================================================================================================
  // tokenizer_span struct
  // ===================================================================================================================
  template <typename LABEL_T>
  struct tokenizer_span {
    // =================================================================================================================
    // Member fields
    // =================================================================================================================
    const char * begin;
    const char * end;
    bool         matched;
    LABEL_T      label;

    // =================================================================================================================
    // Constructors
    // =================================================================================================================
    constexpr tokenizer_span(
      const char * bb = nullptr,
      const char * ee = nullptr,
      bool         mm = true,
      LABEL_T      ll = static_cast<LABEL_T>(0)) :
      begin(bb), end(ee), matched(mm), label(ll) {}
      
    // =================================================================================================================
    // Member functions
    // =================================================================================================================
    constexpr size_t length() const {
      // Return the length of the span.
      return end - begin;
    }

    // =================================================================================================================
    constexpr bool empty() const {
      // True iff the span's length is 0.
      return 0 == length();
    }

    // =================================================================================================================
    constexpr bool no_match() const {
      // True iff the span's beginning and end are both nullptr.
      return ! matched;
    }

    // =================================================================================================================
    constexpr char * c_str() const {
      // Return a *NEW* C string containing the string that matched. THE CALLER OWNS THE RETURNED C STRING!
      return create_new_c_str(*this);
    }
      
    // =================================================================================================================
    constexpr operator bool() const {
      // True iff the span is not empty.
      return ! no_match();
    }

  private: 

    // ===================================================================================================================
    // Static functions
    // ===================================================================================================================
    constexpr static char * create_new_c_str(tokenizer_span const & tok) {
      // Create a *new* C string from a span. THE CALLER OWNS THE RETURNED C STRING!

      if (tok.empty()) return nullptr;
  
      const size_t siz  {(tok.length() + 1) * sizeof(char)};
      char * const word {static_cast<char *>(malloc(siz))};

      memcpy(word, tok.begin, siz);

      word[tok.length()] = 0;

      return word;
    }

    // =================================================================================================================
  };
  // ===================================================================================================================
}
// =====================================================================================================================
#endif
