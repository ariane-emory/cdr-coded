#ifndef RESEUNE_POOL
#define RESEUNE_POOL

namespace reseune {
  template <typename T, size_t S>
  class pool {
  public:

    // ===========================================================================================================
    // Typedefs
    // ===========================================================================================================

    using value_type     = T;
    using iterator       = array_forward_iterator<value_type>;
    using const_iterator = array_forward_const_iterator<value_type>;
    
    // ===========================================================================================================
    // Static constants
    // ===========================================================================================================

    static constexpr size_t SIZE = S;

    // ===========================================================================================================
    // Member field
    // ===========================================================================================================

    value_type data[SIZE];
  
    // ===========================================================================================================
    // Operator member functions
    // ===========================================================================================================
    
    // constexpr const value_type & operator [] (size_t const & ix) const {
    //   return data[ix];
    // }

    constexpr value_type & operator [] (size_t const & ix) {
      return data[ix];
    }

    // ===========================================================================================================
    // Iterator-related member functions
    // ===========================================================================================================
    
    inline iterator begin() {
      return iterator { &data[0] };
    }

    inline iterator end() {
      return iterator { &data[SIZE] };
    }
    
    inline const_iterator cbegin() const {
      return const_iterator { &data[0] };
    }

    inline const_iterator cend() const {
      return const_iterator { &data[SIZE] };
    }
    
    // ===========================================================================================================
  };
};

#endif
