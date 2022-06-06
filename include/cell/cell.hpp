#ifndef RESEUNE_CELL
#define RESEUNE_CELL

#include <inttypes.h>
#include <stdio.h>

namespace reseune {
    // Presently T is expected to be some integer where sizeof(T) == sizeof(void *).
    
    template <typename T>
    class cell {
    public:
        typedef T value_type;
        
        static constexpr uintptr_t FLAG_VALUE      = 0b01000000'00000000'00000000'000000ul;
        static constexpr uintptr_t FLAG_LAST_VALUE = 0b10000000'00000000'00000000'000000ul;
        static constexpr uintptr_t FLAG_REST       = 0b11000000'00000000'00000000'000000ul;
        static constexpr uintptr_t MASK_VALUE      = 0b00111111'11111111'11111111'111111ul;
        static constexpr uintptr_t MASK_FLAG       = ~MASK_VALUE;
        
        enum cell_type : uintptr_t {
            element      = FLAG_VALUE,
            last_element = FLAG_LAST_VALUE,
            rest         = FLAG_REST            
        };

        void * _value;

        constexpr cell(value_type const & v, cell_type const & ct = cell_type::element) {
            _value = reinterpret_cast<void *>(v | ct);
        }

        constexpr value_type value() const {
            return reinterpret_cast<value_type>(_value);
        }

        static void print_uint64_t_bits(char const * descr, uintptr_t const & v) {
            printf("%s", descr);

            for (uintptr_t mask = reinterpret_cast<uintptr_t>(0b10000000'00000000'00000000'000000ul), uint8_t ix = 0;
                 mask;
                 mask >>=1) {
                putchar((mask & v) ? '1' : '0');
            }
            putchar('\n');
        }
        
        void describe() const {
            uintptr_t v = reinterpret_cast<uintptr_t>(_value);

            print_uint64_t_bits("Bits:   ", v);
            print_uint64_t_bits("FMask:  ", MASK_FLAG);
            print_uint64_t_bits("VMask:  ", MASK_VALUE);
        }
    };

    typedef cell<uintptr_t> cellu64;
}

#endif
