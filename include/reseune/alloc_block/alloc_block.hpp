#ifndef RESEUNE_ALLOC_BLOCK_H
#define RESEUNE_ALLOC_BLOCK_H

#define ALLOC_HEADER_SZ offsetof(alloc_block, block)

namespace reseune {
  class alloc_block {
  public:
    char *      block;
    size_t      size;

    // constexpr alloc_block(): size(0), block(nullptr) {}
  };
}

#endif
