#ifndef RESEUNE_BASE_ONE_HPP
#define RESEUNE_BASE_ONE_HPP

#include "reseune/util/util.hpp"
#include "reseune/base_one/allocator/allocator.hpp"

#include <inttypes.h>
#include <stdint.h>
#include <cassert>

namespace reseune {
  namespace base_one {
    // ===========================================================================================================

#ifndef RESEUNE_NO_BASE_ONE_MEMORY
    constexpr size_t       MEMORY_WORDS         {1024};
    constexpr size_t       MEMORY_BYTES         {MEMORY_WORDS << 3};
    static    char         MEMORY[MEMORY_BYTES] {0};
#endif

    // ===========================================================================================================

    inline void initialize(bool verbose = true) {
#ifndef RESEUNE_NO_BASE_ONE_MEMORY
      allocator::add_memory(MEMORY, MEMORY_BYTES, verbose);
#else
      printf("initialize must not be called when -DRESEUNE_NO_BASE_ONE_MEMORY.\n");
      assert(false);
#endif
    }
    
    // ===========================================================================================================    
      }
}

#endif
