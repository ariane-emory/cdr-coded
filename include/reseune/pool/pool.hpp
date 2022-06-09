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
    
    struct const_iterator
    {
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::input_iterator_tag;
      
      const_iterator(value_type const * ptr): m_ptr(ptr) {}
      
      value_type const & operator *  () const { return *m_ptr; }
      value_type const * operator -> () const { return m_ptr; }

      // prefix
      const_iterator & operator ++ () {
        m_ptr++;

        return *this;
      }

      // postfix: untested 
      const_iterator operator ++ (int) {
        const_iterator tmp = *this;

        ++(*this);

        return tmp; 
      } 

      friend bool operator == (const const_iterator & a, const const_iterator & b) { return a.m_ptr == b.m_ptr; };
      friend bool operator != (const const_iterator & a, const const_iterator & b) { return a.m_ptr != b.m_ptr; };     

    private:
      value_type const * m_ptr;
    };
    
    // =================================================================================================================
  };
};

#endif
