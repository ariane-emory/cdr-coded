#ifndef RESEUNE_POOL
#define RESEUNE_POOL

namespace reseune {
  template <typename T, size_t S>
  class pool {
  public:

    // =================================================================================================================
    // Typedefs
    // =================================================================================================================

    using value_type = T;

    // =================================================================================================================
    
    template <typename TT>
    struct array_const_iterator : public const_iterator_base<TT>
    {
      using value_type = TT;
      using base_type  = const_iterator_base<value_type>;
      
      constexpr array_const_iterator(value_type const * ptr): base_type(ptr) {}
      
      // prefix
      inline array_const_iterator & operator ++ () {
        base_type::m_value++;

        return *this;
      }

      // postfix: untested 
      inline array_const_iterator operator ++ (int) {
        array_const_iterator tmp = *this;

        ++(*this);

        return tmp; 
      } 
    };

    using const_iterator = array_const_iterator<value_type>;
    
    // =================================================================================================================
    // Static constants
    // =================================================================================================================

    static constexpr size_t SIZE = S;

    // =================================================================================================================
    // Member field
    // =================================================================================================================

    value_type data[SIZE];
  
    // =================================================================================================================
    // Operator member functions
    // =================================================================================================================
    
    constexpr const value_type & operator [] (size_t const & ix) const {
      return data[ix];
    }

    // =================================================================================================================
    // Iterator-related member functions
    // =================================================================================================================
    
    inline const_iterator begin() const {
      return const_iterator { &data[0] };
    }

    inline const_iterator end() const {
      return const_iterator { &data[SIZE] };
    }
    
    // =================================================================================================================
  };
};

#endif
