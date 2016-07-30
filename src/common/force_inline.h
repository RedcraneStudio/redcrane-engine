/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_COMMON_FORCE_INLINE_H
#define REDC_COMMON_FORCE_INLINE_H

  #ifdef REDC_NO_FORCED_INLINING
    #define REDC_FORCE_INLINE inline
  #else
    #ifdef __MSVC__
      #define REDC_FORCE_INLINE __forceinline
    #else
      #define REDC_FORCE_INLINE __attribute((always_inline))
    #endif // __MSVC__

  #endif // REDC_NO_FORCED_INLINING

#endif // Header block
