#ifndef RESEUNE_ALLOCATOR_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_STRATEGIES_HPP

// #include <inttypes.h>
// #include <stdint.h>
// #include <cassert>

// #include "reseune/util/util.hpp"
// #include "reseune/alloc_info/alloc_info.hpp"
// #include "reseune/doubly_linked/doubly_linked.hpp"

// #include "macros.hpp"

namespace reseune {
  // ===========================================================================================================
  
  template <typename T>
  class allocator;

  // ===========================================================================================================

  struct strategies {
    template <typename alloc_node_t>
    struct no_track {
      static inline void commit_block(alloc_node_t & block, VERBOSEARG) {
        block.remove();
      }

      static inline void release_block(alloc_node_t & block, VERBOSEARG) {
        allocator<alloc_node_t>::place_block(block, verbose);
      }

      static inline bool block_is_free(alloc_node_t const & block, VERBOSEARG) {
        return true;
      }
    };

    // =========================================================================================================

    template <typename alloc_node_t>
    struct track_by_marking {
      static inline void commit_block(alloc_node_t & block, VERBOSEARG) {
        block.data.unfree = true;
      }

      static inline void release_block(alloc_node_t & block, VERBOSEARG) {
        block.data.unfree = false;
      }

      static inline bool block_is_free(alloc_node_t const & block, VERBOSEARG) {
        return ! block.data.unfree;
      }
    };

    // =========================================================================================================
  };
}

#endif
