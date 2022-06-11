#ifndef RESEUNE_ALLOC_INFO_H
#define RESEUNE_ALLOC_INFO_H

#define ALLOC_HEADER_SZ offsetof(alloc_info, block)

namespace reseune {
  class alloc_info {
  public:
    size_t      size;
    char *      block;
  };
}

#endif
