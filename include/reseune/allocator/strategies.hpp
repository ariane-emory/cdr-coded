#ifndef RESEUNE_ALLOCATOR_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_STRATEGIES_HPP

namespace reseune {
  // ===========================================================================================================
  
  template <typename T>
  class allocator;

  // ===========================================================================================================

  struct strategies {
    template <typename alloc_info>
    struct base {
      using allocator_type = allocator<alloc_info>;      
      template <typename t> using container = typename allocator_type::container<t>;
      using alloc_node = container<alloc_info>;            
    };

    // =========================================================================================================

    template <typename alloc_info>
    struct no_track : public base<alloc_info> {
      using b = base<alloc_info>;
      
      static inline void commit_block(b::alloc_node & block, VERBOSEARG) {
        block.remove();
      }

      static inline void release_block(b::alloc_node & block, b::alloc_node & head, VERBOSEARG) {
        b::allocator_type::place_block(block, head, verbose);
      }

      static inline bool block_is_free(b::alloc_node const & block, VERBOSEARG) {
        return true;
      }
    };

    // =========================================================================================================

    template <typename alloc_info>
    struct track_by_marking : public base<alloc_info> {
      using b = base<alloc_info>;      

      static inline void commit_block(b::alloc_node & block, VERBOSEARG) {
        block.data.unfree = true;
      }

      static inline void release_block(b::alloc_node & block, b::alloc_node & head, VERBOSEARG) {
        block.data.unfree = false;
      }

      static inline bool block_is_free(b::alloc_node const & block, VERBOSEARG) {
        return ! block.data.unfree;
      }
    };

    // =========================================================================================================
  };
}

#endif
