#ifndef RESEUNE_CELL
#define RESEUNE_CELL

#include <inttypes.h>
#include <stdio.h>

namespace reseune {
    template<typename ENUM_T, uint8_t FBC>
    class cell {
    public:
        static constexpr uint8_t    FLAG_BITS_COUNT      = FBC;

        typedef uintptr_t value_type;
        typedef ENUM_T cell_type;

        inline static constexpr value_type cell_type_to_mask(cell_type const & ct) {
            return ct << VALUE_BITS_COUNT;
        }

        static constexpr uint8_t    VALUE_BITS_COUNT     = 64 - FLAG_BITS_COUNT;
        static constexpr value_type MASK_VALUE           = (1ul << VALUE_BITS_COUNT) - 1;
        static constexpr value_type MASK_FLAG            = ~MASK_VALUE;
        static constexpr value_type FLAG_MASK_VALUE      = cell_type_to_mask(cell_type::element);
        static constexpr value_type FLAG_MASK_LAST_VALUE = cell_type_to_mask(cell_type::last_element);
        static constexpr value_type FLAG_MASK_REST       = cell_type_to_mask(cell_type::rest);

        value_type data;

        inline constexpr cell(value_type const & v, cell_type const & ct = cell_type::element) {
            data = v | cell_type_to_mask(ct);
        }
        
        inline constexpr value_type value() const {
            return data & MASK_VALUE;
        }

        inline constexpr value_type flag() const {
            return data & MASK_FLAG;
        }

        inline constexpr cell_type type() const {
            return static_cast<cell_type>(flag() >> 62); 
        }

        inline constexpr bool type_is(cell_type const & ct) const {
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
            print_bits("C.data:               ", data);
            print_bits("C.flag():             ", flag());
            print_bits("C.value()             ", value());
            print_bits("C.type():             ", type());
            // print("FB:          ", FLAG_BITS_COUNT);
        }
        
    private:
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
                printf(" = %lu\n", tmp);
            else
                putchar('\n');
        }

        static void print(char const * descr, uint8_t const & v) {
            printf("%s %u\n", descr, v);
        }
    };
}

#endif
