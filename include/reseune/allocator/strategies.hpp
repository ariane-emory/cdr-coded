#ifndef RESEUNE_ALLOCATOR_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_STRATEGIES_HPP

namespace reseune {
  // ===========================================================================================================
  
  template <typename T>
  class allocator;

  // ===========================================================================================================

  struct strategies {
    template <typename alloc_info>
    struct no_track {
      using alloc_node_t = doubly_linked<alloc_info>;
      
      using allocator_type = allocator<typename alloc_node_t::value_type>;
      
      static inline void commit_block(alloc_node_t & block, VERBOSEARG) {
        block.remove();
      }

      static inline void release_block(alloc_node_t & block, alloc_node_t & head, VERBOSEARG) {
        allocator_type::place_block(block, head, verbose);
      }

      static inline bool block_is_free(alloc_node_t const & block, VERBOSEARG) {
        return true;
      }
    };

    // =========================================================================================================

    template <typename alloc_info>
    struct track_by_marking {
      using alloc_node_t = doubly_linked<alloc_info>;

      static inline void commit_block(alloc_node_t & block, VERBOSEARG) {
        block.data.unfree = true;
      }

      static inline void release_block(alloc_node_t & block, alloc_node_t & head, VERBOSEARG) {
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
