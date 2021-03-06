// -*- c-backslash-column: 120; fill-column: 120 -*-

#ifndef RESEUNE_CELL_HPP
#define RESEUNE_CELL_HPP

#include <inttypes.h>
#include <stdio.h>
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

    // ===========================================================================================================
    // Typedefs
    // ===========================================================================================================
    
    using value_type = uintptr_t;
    using uchar_type = uint8_t;

    struct const_iterator;
    
    // ===========================================================================================================
    // Static constants
    // ===========================================================================================================
    
    static constexpr uchar_type TAG_BITS_COUNT   {
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
      2
#else
      1
#endif
    };
    static constexpr uchar_type VALUE_BITS_COUNT { (sizeof(value_type) << 3) - TAG_BITS_COUNT };
    static constexpr value_type MASK_VALUE       { (1ul << VALUE_BITS_COUNT) - 1 };
    static constexpr value_type MASK_TAG         { ~MASK_VALUE };

    // ===========================================================================================================
    // tag_type enum class
    // ===========================================================================================================
    
    enum class tag_type : uintptr_t {
      link,
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
      last_element = 1ul << (VALUE_BITS_COUNT + 1),
#endif
      element      = 1ul << VALUE_BITS_COUNT,
    };

    // ===========================================================================================================
    // Member field
    // ===========================================================================================================

    value_type data;
        
    // ===========================================================================================================
    // Constructors
    // ===========================================================================================================

    constexpr cell() : data(0 | static_cast<value_type>(tag_type::link)) {}

    constexpr cell(value_type const & v, tag_type const & ct = tag_type::element) 
      : data(v | static_cast<value_type>(ct)) {}

    constexpr cell(cell const * v, tag_type const & ct = tag_type::link) 
      : data(reinterpret_cast<value_type>(v) | static_cast<value_type>(ct)) {}

// ===============================================================================================================
// DEFINE_LOGIC_EROR_AND_ASSERT
// ===============================================================================================================
    
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
    DEFINE_LOGIC_ERROR_AND_ASSERT(CANNOT_BE_A_LINK, tag_type::link != tag())
    DEFINE_LOGIC_ERROR_AND_ASSERT(MUST_BE_A_LINK,   tag_type::link == tag())
#undef DEFINE_LOGIC_ERROR_AND_ASSERT

    public:
    
    // ===========================================================================================================
    // Accessor member functions
    // ===========================================================================================================
    
    constexpr auto value() const {
      ASSERT_CANNOT_BE_A_LINK();

      return get_value();
    }

    constexpr auto link() const {
      ASSERT_MUST_BE_A_LINK();
      
      return get_link();
    }
        
    constexpr tag_type tag() const {
      return static_cast<tag_type>(data & MASK_TAG); 
    }

    // ===========================================================================================================
    // Predicate member functions
    // ===========================================================================================================

#define DEFINE_TAG_PREDICATE(name)                                              \
    constexpr auto is_ ## name() const {                                        \
      return is_type(tag_type::name);                                           \
    }

    DEFINE_TAG_PREDICATE(element);
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
    DEFINE_TAG_PREDICATE(last_element);
#endif    
    DEFINE_TAG_PREDICATE(link);
#undef DEFINE_TAG_PREDICATE
    
    constexpr auto is_nil() const {
      return *this == nullptr;
    }

    // ===========================================================================================================
    // Operator member functions
    // ===========================================================================================================
    
#ifdef RESEUNE_CELL_WITHOUT_SPACESHIP
    constexpr auto operator == (cell const & that) {
      return data == that.data;
    }

    constexpr auto operator != (cell const & that) {
      return !operator==(that);
    }
#else
    auto operator <=> (cell const &) const = default;
#endif
    
#ifdef RESEUNE_CELL_WITH_DEREFERENCE_OPERATOR
    constexpr 
    cell const &
    operator  * () const {
      ASSERT_MUST_BE_A_LINK();
      
      return *link();
    }
#endif

    // ===========================================================================================================
    // Printing-related member functions
    // ===========================================================================================================
    
    static constexpr auto tag_type_as_c_str(tag_type const & ct) {
      switch (ct) {
      default: return "ERROR";
#define CASE(enum_val) case tag_type::enum_val: return # enum_val;
        CASE(element);
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
        CASE(last_element);
#endif
        CASE(link);
#undef CASE
      }
    }

    static void describe_class() {
      print("VALUE_BITS_COUNT    :", VALUE_BITS_COUNT);
      print("TAG_BITS_COUNT      :", TAG_BITS_COUNT);
      print_bits<false>("MASK_TAG", MASK_VALUE);
      print_bits<false>("MASK_VALUE", MASK_TAG);
    }
        
    void describe_instance() const {
      print_bits<true, false>("cell is at", uintptr(this));
      if (is_type(tag_type::link))
        print_bits("cell.link()", uintptr(link()));
      else
        print_bits("cell.value()", value());
      printf("cell.tag() as c_str : %s\n", tag_type_as_c_str(tag()));
    }

    // ===========================================================================================================
    // Iterator-related member functions
    // ===========================================================================================================
    
    auto begin() const {
      return const_iterator { this };
    }

    auto end() const {
      return const_iterator { nullptr };
    }
    
    // ===========================================================================================================
  private:
    // ===========================================================================================================
    
    constexpr value_type get_value() const {
      return data & MASK_VALUE;
    }

    constexpr cell const * get_link() const {
      return reinterpret_cast<cell const *>(get_value());
    }

    constexpr bool is_type(tag_type const & ct) const {
      return tag() == ct; 
    }

    static void print(char const * descr, uchar_type const & v) {
      printf("%s %u\n", descr, v);
    }
    
  public:
    
    // ===========================================================================================================
    // cell::const_iterator class
    // ===========================================================================================================
    
    class const_iterator : public const_iterator_base<cell>
    {
    public:
      using base_type         = const_iterator_base<cell>;
      using value_type        = cell;
      
      const_iterator(value_type const * ptr) : base_type(ptr) {}
      
      // prefix
      auto operator ++ () {
        next();

        return *this;
      }

      // postfix: untested 
      auto operator ++ (int) {
        const_iterator tmp = *this;
        next();
        return tmp;
      }
      
    private:
      void next() {
        base_type::m_data->ASSERT_CANNOT_BE_A_LINK();
        
#ifdef RESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
        if (base_type::m_data++->is_last_element()) { // Note post-increment!
          base_type::m_data = nullptr;
        }
#else
        if ((++base_type::m_data)->is_nil()) { // Note pre-increment!
          base_type::m_data = nullptr;
        }
#endif
        else if (base_type::m_data->is_link()) {
          do {
            base_type::m_data = base_type::m_data->link();
          } while (base_type::m_data->is_link());
        }
        // else (i.e, if base_type::m_data->is_element()), it's pointer was already
        // pre or post incremented in the prior case's test.
      }
    };    
    // ===========================================================================================================
  };
}
#endif
