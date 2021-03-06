#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "celery_slicer.h"
#include "celery_script.h"

celery_heap_t* slice(celery_script_t* celery, celery_heap_t* heap) {
    slicer_allocate(heap, celery, 0);
    return heap;
}

heap_addr_t slicer_allocate(celery_heap_t* heap, celery_script_t* celery, heap_addr_t parent_addr) {
    celery_heap_entry_value_t addr_value, parrent_addr_value;

    heap_alot(heap, celery->kind);

    addr_value.number_value = heap->here;
    parrent_addr_value.number_value = parent_addr;

    heap_put(heap, addr_value.number_value, CSH_ADDRESS, HEAP_PARENT, parrent_addr_value);
    slicer_iterate_over_body(heap, celery->body, celery->body_size, addr_value.number_value);
    slicer_iterate_over_args(heap, celery->args, celery->args_size, addr_value.number_value);
    return addr_value.number_value;
}

void slicer_iterate_over_body(celery_heap_t* heap, celery_script_t** body, size_t body_size, heap_addr_t prev_addr) {
    int i;
    heap_addr_t my_heap_address;
    for(i = 0; i<body_size; i++) {
        celery_heap_entry_value_t addr_value;
        celery_heap_entry_value_t zero_value;
        zero_value.number_value = 0;
        if(i == 0) {
            addr_value.number_value = prev_addr + 1;
            heap_put(heap, prev_addr, CSH_ADDRESS, HEAP_BODY, addr_value);
            my_heap_address = slicer_allocate(heap, body[i], prev_addr);
            heap_put(heap, prev_addr, CSH_ADDRESS, HEAP_NEXT, zero_value);
        } else {
            my_heap_address = slicer_allocate(heap, body[i], prev_addr);
            addr_value.number_value = my_heap_address;
            heap_put(heap, prev_addr, CSH_ADDRESS, HEAP_NEXT, addr_value);
        }
    }
}

void slicer_iterate_over_args(celery_heap_t* heap, celery_arg_t** args, size_t args_size, heap_addr_t parent_addr) {
    int i;
    for(i = 0; i<args_size; i++) {
      switch(args[i]->type) {
        case CS_CELERYSCRIPT: {
          celery_heap_entry_value_t addr_value;
          addr_value.number_value = slicer_allocate(heap, args[i]->value.celery_value, parent_addr);
          heap_put(heap, parent_addr, CSH_ADDRESS, args[i]->kind, addr_value);
          break;
        }
        case CS_STRING: {
          // This one might be wrong.
          celery_heap_entry_value_t val;
          val.str_value = args[i]->value.str_value;
          heap_put(heap, parent_addr, args[i]->type, args[i]->kind, val);
          break;
        }
        case CS_BOOL: {
          celery_heap_entry_value_t val;
          val.bool_value = args[i]->value.bool_value;
          heap_put(heap, parent_addr, args[i]->type, args[i]->kind, val);
          break;
        }
        case CS_NUMBER: {
          celery_heap_entry_value_t val;
          val.number_value = args[i]->value.number_value;
          heap_put(heap, parent_addr, args[i]->type, args[i]->kind, val);
          break;
        }
        default:
          break;
      }
    }
}
