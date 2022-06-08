#ifndef RESEUNE_POOL
#define RESEUNE_POOL

namespace reseune {
  template <typename T, size_t S>
  class pool {
  public:
    using value_type = T;
  
    static constexpr size_t SIZE = S;

    value_type data[SIZE];
  
    constexpr value_type const & operator [] (size_t const & ix) const {
      return data[ix];
    }
  };
}

#endif
