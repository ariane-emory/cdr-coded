#ifndef RESEUNE_ALLOC_BLOCK_H
#define RESEUNE_ALLOC_BLOCK_H

#define ALLOC_HEADER_SZ offsetof(alloc_block, block)

namespace reseune {
  class alloc_block {
  public:
    size_t      size;
    char *      block;

    constexpr alloc_block(): size(0), block(nullptr) {}
  };
}

#endif
