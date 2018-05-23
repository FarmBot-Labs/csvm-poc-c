#ifndef CSVM_H
#define CSVM_H

#include <inttypes.h>
#include "celery_heap.h"


#define NUMBER_REGISTERS 16
#define REGISTER_PC 0
#define REGISTER_SP 1

typedef uint32_t csvm_register_t;

typedef struct Csvm {
    csvm_register_t registers[NUMBER_REGISTERS];
    celery_heap_t* heap;
} csvm_t;

csvm_t* csvm_init();
#endif
