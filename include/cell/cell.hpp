#ifndef AECC_CELL
#define AECC_CELL

#include <inttypes.h>

namespace reseune {
    template <typename T>
    class cell {
    public:
        typedef T value_type;
        
        static constexpr uint64_t FLAG_MASK_VALUE      = 0b01000000'00000000'00000000'000000;
        static constexpr uint64_t FLAG_MASK_LAST_VALUE = 0b10000000'00000000'00000000'000000;
        static constexpr uint64_t FLAG_MASK_REST       = 0b11000000'00000000'00000000'000000;

        enum class cell_type {
            value      = FLAG_MASK_VALUE,
            last_value = FLAG_MASK_LAST_VALUE,
            rest       = FLAG_MASK_REST            
        };

        void * value;

        cell(value_type const & _value) {            
        }
    };

    typedef cell<uint64_t> cellu64;
}

#endif
