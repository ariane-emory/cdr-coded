#ifndef RESEUNE_CELL
#define RESEUNE_CELL

#include <inttypes.h>
#include <stdio.h>
#include <bit>
#include <cassert>
#include <stdexcept>

namespace reseune {
  class cell {
  public:
    typedef uintptr_t value_type;
    typedef uint8_t uchar_type;

    enum class cell_type : uintptr_t {
      element = 1,
      last_element,
      rest
    };

    static constexpr uchar_type FLAG_BITS_COUNT      = 2;
    static constexpr uchar_type VALUE_BITS_COUNT     = (sizeof(value_type) << 3) - FLAG_BITS_COUNT;
    static constexpr value_type MASK_VALUE           = (1ul << VALUE_BITS_COUNT) - 1;
    static constexpr value_type MASK_FLAG            = ~MASK_VALUE;
    static constexpr value_type FLAG_MASK_VALUE      = static_cast<value_type>(cell_type::element)      << VALUE_BITS_COUNT;
    static constexpr value_type FLAG_MASK_LAST_VALUE = static_cast<value_type>(cell_type::last_element) << VALUE_BITS_COUNT;
    static constexpr value_type FLAG_MASK_REST       = static_cast<value_type>(cell_type::rest)         << VALUE_BITS_COUNT;

    value_type data;

    inline constexpr
    cell(value_type const & v, cell_type const & ct) 
      : data(v | (static_cast<value_type>(ct) << VALUE_BITS_COUNT)) {
      // if (cell_type::rest == ct)
      //   throw assert_failure("ct == rest");
    }
    
    inline constexpr
    cell(cell * const v, cell_type const & ct) 
      : data(std::bit_cast<value_type>(v) | (static_cast<value_type>(ct) << VALUE_BITS_COUNT)) {
      // if (cell_type::rest != ct)
      //   throw assert_failure("ct != rest");
    }

    inline constexpr
    cell() 
      : data(0 | (static_cast<value_type>(cell_type::rest) << VALUE_BITS_COUNT)) {}
    
#define LOGIC_ERROR(name, expr)                                                 \
    class name   : public std::logic_error {                                    \
    public:                                                                     \
    name() : std::logic_error(# name) {};                                       \
    };                                                                          \
    inline constexpr                                                            \
    void assert_ ## name() const {                                              \
      if (! (expr))                                                             \
        throw name {};                                                          \
    }
    
    LOGIC_ERROR(cannot_be_a_rest, cell_type::rest != type())
    LOGIC_ERROR(must_be_a_rest,   cell_type::rest == type())
    
    inline constexpr
    value_type value() const  {
      assert_cannot_be_a_rest();

      return get_value();
    }

    inline constexpr
    cell const * rest() const  {
      assert_must_be_a_rest();
      
      return get_rest();
    }
        
    inline constexpr
    value_type flag() const {
      return data & MASK_FLAG;
    }

    inline constexpr
    cell_type type() const {
      return static_cast<cell_type>(flag() >> 62); 
    }

    inline constexpr
    bool is_type(cell_type const & ct) const {
      return type() == ct; 
    }
        
    static inline
    void describe_class() {
      print("VB:                   ", VALUE_BITS_COUNT);
      print("FB:                   ", FLAG_BITS_COUNT);
      print_bits("MASK_FLAGS:           ", MASK_VALUE, false);
      print_bits("MASK_VALUE:           ", MASK_FLAG, false);
      print_bits("FLAG_MASK_VALUE:      ", FLAG_MASK_VALUE, false);
      print_bits("FLAG_MASK_LAST_VALUE: ", FLAG_MASK_LAST_VALUE, false);
      print_bits("FLAG_MASK_REST:       ", FLAG_MASK_REST, false);
      putchar('\n');
    }
        
    inline
    void describe_instance() const {
      // printf("Cell at:                                                                                           %018p\n", this);
      print_bits("cell @                ", reinterpret_cast<uintptr_t>(this));
      print_bits("cell.data:            ", data);
      print_bits("cell.flag():          ", flag());
      print_bits("cell.type():          ", type());
      if (is_type(cell_type::rest))
        print_bits("cell.rest():          ", reinterpret_cast<uintptr_t>(rest()));
      else
        print_bits("cell.value()          ", value());
      putchar('\n');
    }
        
  private:
    inline constexpr
    value_type get_value() const {
      return data & MASK_VALUE;
    }

    inline constexpr
    cell const * get_rest() const {
      return std::bit_cast<cell const *>(get_value());
    }

    template <typename T>
    static inline
    void print_bits(
      char const * descr, T const & v, bool const & print_int = true) {
      printf("%s 0b", descr);

      value_type tmp = static_cast<value_type>(v);
            
      {
        uchar_type ix = 0;
            
        for (value_type mask = 0b10000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000ul;
             mask;
             mask >>= 1) {
          putchar((mask & tmp) ? '1' : '0');

          ++ix %= 8;
          if (0 == ix && mask > 1)
            putchar('\'');
        }
      }
            
      if (print_int)
        printf(" = 0x%016lx = % 24lu\n", tmp, tmp);
      else
        putchar('\n');
    }

    static inline
    void print(char const * descr, uchar_type const & v) {
      printf("%s %u\n", descr, v);
    }
  };

  // static constexpr cell nil { static_cast<reseune::cell::value_type>(0), reseune::cell::cell_type::rest };

  struct conspair {
    cell car, cdr;

    inline constexpr
    conspair() {}
  };

}

#endif
