#ifndef RESEUNE_CELL
#define RESEUNE_CELL

#include <inttypes.h>
#include <stdio.h>

namespace reseune {
    // Presently T is expected to be some integer where sizeof(T) == sizeof(void *).
    
    class cell {
    public:
        typedef uintptr_t value_type;
        
        static constexpr uintptr_t FLAG_VALUE      = 0b01000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000ul;
        static constexpr uintptr_t FLAG_LAST_VALUE = 0b10000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000ul;
        static constexpr uintptr_t FLAG_REST       = 0b11000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000ul;
        static constexpr uintptr_t MASK_VALUE      = 0b00111111'11111111'11111111'11111111'11111111'11111111'11111111'11111111ul;
        static constexpr uintptr_t MASK_FLAG       = ~MASK_VALUE;
        
        enum cell_type : uintptr_t {
            element      = FLAG_VALUE,
            last_element = FLAG_LAST_VALUE,
            rest         = FLAG_REST            
        };

        uintptr_t _value;

        constexpr cell(value_type const & v, cell_type const & ct = cell_type::element) {
            _value = v | ct;
        }

        constexpr value_type value() const {
            return _value & MASK_VALUE;
        }

        constexpr value_type flag() const {
            return _value & MASK_FLAG;
        }

        constexpr value_type type() const {
            return type() >> 1; 
        }
        
        void describe() const {
            print_uint64_t_bits("Bits:  ", _value);
            print_uint64_t_bits("VMask  ", MASK_VALUE, false);
            print_uint64_t_bits("FMask: ", MASK_FLAG, false);
            print_uint64_t_bits("VBits  ", value());
            print_uint64_t_bits("FBits: ", flag());
            // print_uint64_t_bits("Type:  ", type());
            //printf("Type:  %u", type());
        }
        
    private:
        //template <typename T>
        static void print_uint64_t_bits(char const * descr, uintptr_t const & v, bool const & print_int = true) {
            printf("%s 0b", descr);

            uint8_t ix = 0;
            
            for (uintptr_t mask = reinterpret_cast<uintptr_t>(0b10000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000ul);
                 mask;
                 mask >>= 1) {
                putchar((mask & v) ? '1' : '0');

                ++ix %= 8;
                if (0 == ix && mask > 1)
                    putchar('\'');
            }

            if (print_int)
                printf(" = %lu\n", v);
            else
                putchar('\n');
        }
    };
}

#endif
