#ifndef RESEUNE_HPP
#define RESEUNE_HPP

namespace reseune {
//   static constexpr bool THROW =
// #ifdef RESEUNE_THROW
//     true
// #else
//     false
// #endif
//     ;

  static constexpr bool WARN =
#ifdef RESEUNE_WARN
    true
#else
    false
#endif
    ;
}

#endif
