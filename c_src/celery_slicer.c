#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "celery_slicer.h"
#include "celery_script.h"

celery_heap_t* slice(celery_script_t* celery) {
    celery_heap_t* heap = heap_init();
    slicer_allocate(heap, celery, 0);
    return heap;
}

int slicer_allocate(celery_heap_t* heap, celery_script_t* celery, int parent_addr) {
    heap_alot(heap, celery->kind);
    int addr = heap->here;
    heap_put(heap, addr, CSH_ADDRESS, HEAP_PARENT, &parent_addr);
    slicer_iterate_over_body(heap, celery->body, celery->body_size, addr);
    slicer_iterate_over_args(heap, celery->args, celery->args_size, addr);
    return addr;
}

void slicer_iterate_over_body(celery_heap_t* heap, celery_script_t** body, int body_size, int prev_addr) {
    int i, my_heap_address;
    for(i = 0; i<body_size; i++) {
        if(i == 0) {
            int value = prev_addr + 1;
            heap_put(heap, prev_addr, CSH_ADDRESS, HEAP_BODY, &value);
            my_heap_address = slicer_allocate(heap, body[i], prev_addr);
            heap_put(heap, prev_addr, CSH_NULL, HEAP_NEXT, 0);
        } else {
            my_heap_address = slicer_allocate(heap, body[i], prev_addr);
            heap_put(heap, prev_addr, CSH_ADDRESS, HEAP_NEXT, &my_heap_address);
        }
    }
}

void slicer_iterate_over_args(celery_heap_t* heap, celery_arg_t** args, int args_size, int parent_addr) {
    int i;
    for(i = 0; i<args_size; i++) {
        if(args[i]->type == CS_CELERYSCRIPT) {
            int buf_size = strlen(HEAP_LINK) + 1 + strlen(args[i]->kind) + 1;
            char* buf = malloc(buf_size);
            snprintf(buf, buf_size, "%s_%s", HEAP_LINK, args[i]->kind);
            int addr = slicer_allocate(heap, args[i]->value, parent_addr);
            heap_put(heap, parent_addr, CSH_ADDRESS, buf, &addr);
        } else {
            heap_put(heap, parent_addr, args[i]->type, args[i]->kind, args[i]->value);
        }
    }
}
