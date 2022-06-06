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
        
        static constexpr uint64_t FLAG_MASK_VALUE      = 0b01000000'00000000'00000000'000000ul;
        static constexpr uint64_t FLAG_MASK_LAST_VALUE = 0b10000000'00000000'00000000'000000ul;
        static constexpr uint64_t FLAG_MASK_REST       = 0b11000000'00000000'00000000'000000ul;

        enum class cell_type {
            value      = FLAG_MASK_VALUE,
            last_value = FLAG_MASK_LAST_VALUE,
            rest       = FLAG_MASK_REST            
        };

        void * _value;

        constexpr cell(value_type const & v, cell_type const & type = cell_type::value)
            : _value(reinterpret_cast<void *>(v)) {}

        constexpr value_type value() const {
            return reinterpret_cast<value_type>(_value);
        }

        void print_bits() const {
            uintptr_t v = reinterpret_cast<uintptr_t>(_value);
            
            for (uintptr_t mask = reinterpret_cast<uintptr_t>(0b10000000'00000000'00000000'000000ul);
                 mask;
                 mask >>=1) {
                putchar((mask & v) ? '1' : '0');
            }
        }
    };

    typedef cell<uint64_t> cellu64;
}

#endif
