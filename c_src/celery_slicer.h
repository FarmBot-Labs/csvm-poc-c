#ifndef CELERYSLICER_H
#define CELERYSLICER_H

#include "celery_script.h"
#include "celery_heap.h"

celery_heap_t* slice(celery_script_t* celery, celery_heap_t* heap);
heap_addr_t slicer_allocate(celery_heap_t* heap, celery_script_t* celery, heap_addr_t addr);
void slicer_iterate_over_body(celery_heap_t* heap, celery_script_t** body, size_t body_size, heap_addr_t prev_addr);
void slicer_iterate_over_args(celery_heap_t* heap, celery_arg_t** args, size_t args_size, heap_addr_t addr);

#endif
