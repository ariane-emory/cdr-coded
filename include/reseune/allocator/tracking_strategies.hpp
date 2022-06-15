#ifndef RESEUNE_ALLOCATOR_ALLOCATOR_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_ALLOCATOR_STRATEGIES_HPP

#include "placement_strategies.hpp"
#include "macros.hpp" // include last
#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/alloc_info_with_unfree_flag/alloc_info_with_unfree_flag.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"
#include <tuple>

// =============================================================================================================
namespace reseune {

  // ===========================================================================================================
  namespace tracking_strategies {

    // =========================================================================================================
    template <
      template <typename> typename container,
      template <template <typename> typename> typename placement>
    struct standard {};
    
    // =========================================================================================================
    template <
      template <template <typename> typename> typename tplacement>
    struct standard<doubly_linked, tplacement> {
      template <typename t> using container = doubly_linked<t>;
      
      static inline void commit_block(
        container<alloc_info> & block,
        VERBOSEARG) {
        (std::ignore = verbose);
        block.remove();
      }
            
      static inline void release_block(
        container<alloc_info> & block,
        container<alloc_info> & head,
        VERBOSEARG) {
        tplacement<container>::place(block, head, verbose);
      }

      static inline bool block_is_free(
        container<alloc_info> const & block,
        VERBOSEARG) {
        (std::ignore = verbose) = block;
        return true;
      }

      static inline void commit_block(
        container<alloc_info_with_unfree_flag> & block,
        VERBOSEARG) {
        (std::ignore = verbose);
        block.data.unfree = true;
      }

      static inline void release_block(
        container<alloc_info_with_unfree_flag> & block,
        container<alloc_info_with_unfree_flag> & head,
        VERBOSEARG) {
        (std::ignore = verbose) = head;        
        block.data.unfree = false;
      }

      static inline bool block_is_free(
        container<alloc_info_with_unfree_flag> const & block,
        VERBOSEARG) {
        (std::ignore = verbose);        
        return ! block.data.unfree;
      }
    };

    // =========================================================================================================
  };
// ===========================================================================================================
}
// =============================================================================================================
#endif
