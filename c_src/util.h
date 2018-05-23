#ifndef CSVM_UTIL_H
#define CSVM_UTIL_H

#if defined(DEBUG)
#define debug_print(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)
#else
#define debug_print(fmt, args...) /* Don't do anything in release builds */
#endif

#endif
