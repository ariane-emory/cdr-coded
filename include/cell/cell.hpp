#ifndef RESEUNE_CELL
#define RESEUNE_CELL

#include <inttypes.h>
#include <stdio.h>
#include <bit>
#include <cassert>

namespace reseune {
    class cell {
    public:
        typedef uintptr_t value_type;

        enum class cell_type : uintptr_t {
            element = 1,
            last_element,
            rest 
        };

        static constexpr uint8_t    FLAG_BITS_COUNT      = 2;
        static constexpr uint8_t    VALUE_BITS_COUNT     = (sizeof(value_type) << 3) - FLAG_BITS_COUNT;
        static constexpr value_type MASK_VALUE           = (1ul << VALUE_BITS_COUNT) - 1;
        static constexpr value_type MASK_FLAG            = ~MASK_VALUE;
        static constexpr value_type FLAG_MASK_VALUE      = static_cast<value_type>(cell_type::element)      << VALUE_BITS_COUNT;
        static constexpr value_type FLAG_MASK_LAST_VALUE = static_cast<value_type>(cell_type::last_element) << VALUE_BITS_COUNT;
        static constexpr value_type FLAG_MASK_REST       = static_cast<value_type>(cell_type::rest)         << VALUE_BITS_COUNT;

        value_type data;

        inline constexpr cell(value_type const & v, cell_type const & ct) {
            data = v | (static_cast<value_type>(ct) << VALUE_BITS_COUNT);
        }
        
        inline constexpr value_type value() const {
            assert(is_type(cell_type::element) || is_type(cell_type::last_element));
            
            return get_value();
        }

        inline constexpr cell const * rest() const {
            assert(is_type(cell_type::rest));
            
            return get_rest();
        }
        
        inline constexpr value_type flag() const {
            return data & MASK_FLAG;
        }

        inline constexpr cell_type type() const {
            return static_cast<cell_type>(flag() >> 62); 
        }

        inline constexpr bool is_type(cell_type const & ct) const {
            return type() == ct; 
        }
        
        inline void describe() const {
            print("VB:                   ", VALUE_BITS_COUNT);
            print("FB:                   ", FLAG_BITS_COUNT);
            print_bits("MASK_FLAGS:           ", MASK_VALUE, false);
            print_bits("MASK_VALUE:           ", MASK_FLAG, false);
            print_bits("FLAG_MASK_VALUE:      ", FLAG_MASK_VALUE, false);
            print_bits("FLAG_MASK_LAST_VALUE: ", FLAG_MASK_LAST_VALUE, false);
            print_bits("FLAG_MASK_REST:       ", FLAG_MASK_REST, false);
            printf("Cell at:                                                                                           %018p\n", this);
            print_bits("Cell at:              ", reinterpret_cast<uintptr_t>(this));
            print_bits("C.data:               ", data);
            print_bits("C.flag():             ", flag());
            print_bits("C.type():             ", type());
            if (is_type(cell_type::rest))
                print_bits("C.rest():             ", reinterpret_cast<uintptr_t>(rest()));
            else
                print_bits("C.value()             ", value());

            // print("FB:          ", FLAG_BITS_COUNT);
        }
        
    private:
        inline constexpr value_type get_value() const {
            return data & MASK_VALUE;
        }

        inline constexpr cell const * get_rest() const {
            return std::bit_cast<cell const *>(get_value());
        }

        template <typename T>
        static void print_bits(char const * descr, T const & v, bool const & print_int = true) {
            printf("%s 0b", descr);

            value_type tmp = static_cast<value_type>(v);
            
            {
                uint8_t ix = 0;
            
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
                printf(" = 0x%016lx = %lu\n", tmp, tmp);
            else
                putchar('\n');
        }

        static void print(char const * descr, uint8_t const & v) {
            printf("%s % 16u\n", descr, v);
        }
    };
}

#endif
