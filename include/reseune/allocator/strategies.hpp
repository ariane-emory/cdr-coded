#ifndef RESEUNE_ALLOCATOR_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_STRATEGIES_HPP

namespace reseune {
  // ===========================================================================================================
  
  template <typename T, template <typename> typename S>
  class allocator;

  // ===========================================================================================================

  struct strategies {

    // =========================================================================================================

  private:
    
    // template <typename alloc_info>
    // struct base {
    // protected:
    // };

    // =========================================================================================================

  public:

    // =========================================================================================================
    
    template <typename alloc_info>
    class no_track /* : private base<alloc_info> */ {
    public:
      using allocator_type = allocator<alloc_info, no_track>;
      template <typename t> using container = typename allocator_type::container<t>;
      using alloc_node = container<alloc_info>;            


      static inline void commit_block(alloc_node & block, VERBOSEARG) {
        block.remove();
      }

      static inline void release_block(alloc_node & block, alloc_node & head, VERBOSEARG) {
        allocator_type::place_block(block, head, verbose);
      }

      static inline bool block_is_free(alloc_node const & block, VERBOSEARG) {
        return true;
      }
    };

    // =========================================================================================================

    // template <typename alloc_info>
    // class track_by_marking : private base<alloc_info> {
    // private:
    //   using b = base<alloc_info>;

    // public:
    //   static inline void commit_block(b::alloc_node & block, VERBOSEARG) {
    //     block.data.unfree = true;
    //   }

    //   static inline void release_block(b::alloc_node & block, b::alloc_node & head, VERBOSEARG) {
    //     block.data.unfree = false;
    //   }

    //   static inline bool block_is_free(b::alloc_node const & block, VERBOSEARG) {
    //     return ! block.data.unfree;
    //   }
    // };

    // =========================================================================================================
  };
}

#endif
