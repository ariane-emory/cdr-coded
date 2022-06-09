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

    struct const_iterator;
    
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
    
    constexpr value_type const & operator [] (size_t const & ix) const {
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
    // pool<T, S>::const_iterator class
    // =================================================================================================================
    
    struct const_iterator : public const_iterator_base<value_type>
    {
      using base_type = const_iterator_base<value_type>;
      
      constexpr const_iterator(value_type const * ptr): base_type(ptr) {}
      
      // prefix
      inline const_iterator & operator ++ () {
        base_type::m_value++;

        return *this;
      }

      // postfix: untested 
      inline const_iterator operator ++ (int) {
        const_iterator tmp = *this;

        ++(*this);

        return tmp; 
      } 
    };
    
    // =================================================================================================================
  };
};

#endif
