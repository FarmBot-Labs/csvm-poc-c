#ifndef CSVM_UTIL_H
#define CSVM_UTIL_H

#if defined(DEBUG)
  #include <stdio.h>
  #define debug_print(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
      __FILE__, __LINE__, __func__, ##args)

  #define debug_print_q(fmt, args...) fprintf(stderr, fmt, ##args)

#else

  #define debug_print(fmt, args...)
  #define debug_print_q(fmt, args...)

#endif

#endif
