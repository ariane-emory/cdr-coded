#ifndef RESEUNE_CELL
#define RESEUNE_CELL

#include <inttypes.h>
#include <stdio.h>

namespace reseune {
    template<typename TT>
    class cell {
    public:
        typedef uintptr_t value_type;

        enum cell_type : uintptr_t {
            element      = 1,
            last_element = 2,
            rest         = 3
        };

        inline static constexpr uintptr_t cell_type_to_mask(cell_type const & ct) {
            return ct << VALUE_BITS;
        }

        static constexpr uint8_t   FLAG_BITS            = 2;
        static constexpr uint8_t   VALUE_BITS           = 64 - FLAG_BITS;
        static constexpr uintptr_t MASK_VALUE           = 0b00111111'11111111'11111111'11111111'11111111'11111111'11111111'11111111ul;
        static constexpr uintptr_t MASK_FLAG            = ~MASK_VALUE;
        static constexpr uintptr_t FLAG_MASK_VALUE      = cell_type_to_mask(cell_type::element);
        static constexpr uintptr_t FLAG_MASK_LAST_VALUE = cell_type_to_mask(cell_type::last_element);
        static constexpr uintptr_t FLAG_MASK_REST       = cell_type_to_mask(cell_type::rest);

        uintptr_t data;

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
            return static_cast<cell_type>(flag() > 62); 
        }
        
        inline void describe() const {
            print("VB:                   ", VALUE_BITS);
            print("FB:                   ", FLAG_BITS);
            print_bits("MASK_FLAGS:           ", MASK_VALUE, false);
            print_bits("MASK_VALUE:           ", MASK_FLAG, false);
            print_bits("FLAG_MASK_VALUE:      ", FLAG_MASK_VALUE, false);
            print_bits("FLAG_MASK_LAST_VALUE: ", FLAG_MASK_LAST_VALUE, false);
            print_bits("FLAG_MASK_REST:       ", FLAG_MASK_REST, false);
            print_bits("C.data:               ", data);
            print_bits("C.flag():             ", flag());
            print_bits("C.value()             ", value());
            print_bits("C.type():             ", type());
            // print("FB:          ", FLAG_BITS);
        }
        
    private:
        template <typename T>
        static void print_bits(char const * descr, T const & v, bool const & print_int = true) {
            printf("%s 0b", descr);

            uintptr_t tmp = static_cast<uintptr_t>(v);
            
            {
                uint8_t ix = 0;
            
                for (uintptr_t mask = 0b10000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000ul;
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
