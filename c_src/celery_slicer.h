#ifndef CELERYSLICER_H
#define CELERYSLICER_H

#include "celery_script.h"
#include "celery_heap.h"

celery_heap_t* slice(celery_script_t* celery);
int slicer_allocate(celery_heap_t* heap, celery_script_t* celery, int addr);
void slicer_iterate_over_body(celery_heap_t* heap, celery_script_t** body, int body_size, int prev_addr);
void slicer_iterate_over_args(celery_heap_t* heap, celery_arg_t** args, int args_size, int addr);

#endif
