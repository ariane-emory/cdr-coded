// -*- c-backslash-column: 120; fill-column: 120 -*-

#ifndef RESEUNE_CELL_HPP
#define RESEUNE_CELL_HPP

#include <inttypes.h>
#include <stdio.h>
#include <bit>
#include <iterator>

#include "reseune/reseune.hpp"

#ifdef __EXCEPTIONS
#include <stdexcept>
#endif

#ifndef RESEUNE_CELL_WITHOUT_SPACESHIP
#include <compare>
#endif

namespace reseune {
  class cell {
  public:

// =====================================================================================================================
// Typedefs
// =====================================================================================================================
    
    using value_type = uintptr_t;
    using uchar_type = uint8_t;

// =====================================================================================================================
// Static constants
// =====================================================================================================================
    
    static constexpr uchar_type TAG_BITS_COUNT      {
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
      2
#else
      1
#endif
    };
    static constexpr uchar_type VALUE_BITS_COUNT    { (sizeof(value_type) << 3) - TAG_BITS_COUNT };
    static constexpr value_type MASK_VALUE          { (1ul << VALUE_BITS_COUNT) - 1 };
    static constexpr value_type MASK_TAG            { ~MASK_VALUE };

// =====================================================================================================================
// tag_t enum class
// =====================================================================================================================
    
    enum class tag_t : uintptr_t {
      link,
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
      last_element = 1ul << (VALUE_BITS_COUNT + 1),
#endif
      element = 1ul << VALUE_BITS_COUNT,
    };

// =====================================================================================================================
// Member field
// =====================================================================================================================

    value_type data;
        
// =====================================================================================================================
// Constructors
// =====================================================================================================================

    constexpr
    cell() 
      : data(0 | static_cast<value_type>(tag_t::link)) {}

    constexpr
    cell(value_type const & v, tag_t const & ct = tag_t::element
         ) 
      : data(v | static_cast<value_type>(ct)) {}

    constexpr
    cell(cell const * v, tag_t const & ct = tag_t::link) 
      : data(reinterpret_cast<value_type>(v) | static_cast<value_type>(ct)) {}

// =====================================================================================================================
// DEFINE_LOGIC_EROR_AND_ASSERT
// =====================================================================================================================
    
#ifdef __EXCEPTIONS
#define DEFINE_LOGIC_ERROR_AND_ASSERT(name, expr)                               \
    class name : public std::logic_error {                                      \
    public:                                                                     \
    name() : std::logic_error(# name) {};                                       \
    };                                                                          \
                                                                                \
    constexpr                                                                   \
    void                                                                        \
    ASSERT_ ## name() const {                                                   \
      if (! (expr)) [[unlikely]]                                                \
        throw name {};                                                          \
    }
#else
#define DEFINE_LOGIC_ERROR_AND_ASSERT(name, expr)                               \
    constexpr                                                                   \
    void                                                                        \
    ASSERT_ ## name() const {                                                   \
      if (! (expr)) [[unlikely]]                                                \
        printf("WARNING: " # name  "\n");                                       \
    }
#endif
  private:
    DEFINE_LOGIC_ERROR_AND_ASSERT(CANNOT_BE_A_LINK, tag_t::link != tag())
    DEFINE_LOGIC_ERROR_AND_ASSERT(MUST_BE_A_LINK,   tag_t::link == tag())
#undef DEFINE_LOGIC_ERROR_AND_ASSERT

    public:
    
    // =================================================================================================================
    // Accessor member functions
    // =================================================================================================================
    
    constexpr
    value_type value() const {
      ASSERT_CANNOT_BE_A_LINK();

      return get_value();
    }

    constexpr
    cell const *
    link() const {
      ASSERT_MUST_BE_A_LINK();
      
      return get_link();
    }
        
    constexpr
    tag_t tag() const {
      return static_cast<tag_t>(data & MASK_TAG); 
    }

    // =================================================================================================================
    // Predicate member functions
    // =================================================================================================================
    
    constexpr
    bool is_element() const {
      return is_type(tag_t::element);
    }

#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
    constexpr
    bool is_last_element() const {
      return is_type(tag_t::last_element);
    }
#endif

    constexpr
    bool is_link() const {
      return is_type(tag_t::link);
    }

    constexpr
    bool is_nil() const {
      return *this == cell {};
    }

    // =================================================================================================================
    // Operator member functions
    // =================================================================================================================
    
#ifdef RESEUNE_CELL_WITHOUT_SPACESHIP
    constexpr
    bool operator == (cell const & that) {
      return data == that.data;
    }

    constexpr
    bool operator != (cell const & that) {
      return !operator==(that);
    }
#else
    auto operator <=> (cell const  &) const = default;
#endif
    
#ifdef RESEUNE_CELL_WITH_DEREFERENCE_OPERATOR
    constexpr
    cell const &
    operator  * () const {
      ASSERT_MUST_BE_A_LINK();
      return *link();
    }
#endif

    // =================================================================================================================
    // Printing-related member functions
    // =================================================================================================================
    
    static constexpr
    char const * const
    tag_t_as_c_str(tag_t const & ct) {
      switch (ct) {
      default: return "ERROR";
#define CASE(enum_val) case tag_t::enum_val: return # enum_val;
        CASE(element);
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
        CASE(last_element);
#endif
        CASE(link);
#undef CASE
      }
    }

    static
    void describe_class() {
      print("VALUE_BITS_COUNT:          ", VALUE_BITS_COUNT);
      print("TAG_BITS_COUNT:            ", TAG_BITS_COUNT);
      print_bits("MASK_TAG:                  ", MASK_VALUE, false);
      print_bits("MASK_VALUE:                ", MASK_TAG, false);
      putchar('\n');
    }
        
    void describe_instance() const {
      print_bits("cell is at                 ", reinterpret_cast<uintptr_t>(this), true, false);
      // print_bits("cell.data:                 ", data);
      // print_bits("cell.tag():                ", tag());
      printf("cell.tag() as c_str:        %s\n", tag_t_as_c_str(tag()));
      if (is_type(tag_t::link))
        print_bits("cell.link():               ", reinterpret_cast<uintptr_t>(link()));
      else
        print_bits("cell.value()               ", value());
      putchar('\n');
    }

    // =================================================================================================================
    // cell::const_iterator class
    // =================================================================================================================
    
    struct const_iterator
    {
      using value_type        = cell;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::input_iterator_tag;
      
      const_iterator(value_type const * ptr): m_ptr(ptr) {}
      
      value_type const & operator *  () const { return *m_ptr; }
      value_type const * operator -> () const { return m_ptr; }

      void next() {
        m_ptr->ASSERT_CANNOT_BE_A_LINK();
        
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
        if (m_ptr++->is_last_element()) { // Note post-increment!
          m_ptr = nullptr;
        }
#else
        if ((++m_ptr)->is_nil()) { // Note pre-increment!
          m_ptr = nullptr;
        }
#endif
        else if (m_ptr->is_link()) {
          do {
            m_ptr = m_ptr->link();
          } while (m_ptr->is_link());
        }
        // else (i.e, if m_ptr->is_element()), it's pointer was already
        // pre or post incremented in the prior case's test.
      }

      // prefix
      const_iterator & operator ++ () {
        next();

        return *this;
      }

      // postfix: untested 
      const_iterator operator ++ (int) {
        const_iterator tmp = *this;
        next();
        return tmp;
      } 

      friend bool operator == (const const_iterator & a, const const_iterator & b) { return a.m_ptr == b.m_ptr; };
      friend bool operator != (const const_iterator & a, const const_iterator & b) { return a.m_ptr != b.m_ptr; };     

    private:
      value_type const * m_ptr;
    };
    
    const_iterator begin() const {
      return const_iterator { this };
    }

    const_iterator end() const {
      return const_iterator { nullptr };
    }

    // =================================================================================================================
  private:
    // =================================================================================================================
    
    constexpr
    value_type get_value() const {
      return data & MASK_VALUE;
    }

    constexpr
    cell const *
    get_link() const {
      return std::bit_cast<cell const *>(get_value());
    }

    constexpr
    bool is_type(tag_t const & ct) const {
      return tag() == ct; 
    }

    template <typename T>
    static
    void print_bits(
      char const * descr,
      T const & v,
      bool const & show_int = true,
      bool const & show_bits = true) {

      printf(
        show_bits
        ? "%s 0b"
        : "%s   ",
        descr);

      value_type tmp { static_cast<value_type>(v) };
            
      {
        uchar_type ix { 0 };
            
        for (value_type mask { 0b10000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000ul };
             mask;
             mask >>= 1) {
          if (show_bits)
            putchar((mask & tmp) ? '1' : '0');
          else
            putchar(' ');

          ++ix %= 8;

          if (0 == ix && mask > 1) [[unlikely]]
            putchar(show_bits
                    ? '\''
                    : ' ');
        }
      }
            
      if (show_int) [[likely]]
        printf(show_bits
               ? " = 0x%016lx = % 20lu\n"
               : "   0x%016lx = % 20lu\n",
               tmp,
               tmp);
      else [[unlikely]]
        putchar('\n');
    }

    static
    void print(char const * descr, uchar_type const & v) {
      printf("%s %u\n", descr, v);
    }
    
    // =================================================================================================================
  };
}
#endif
