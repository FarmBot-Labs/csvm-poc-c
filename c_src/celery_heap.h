#ifndef CELERYHEAP_H
#define CELERYHEAP_H

#include <stdbool.h>

typedef size_t heap_key_t;
typedef size_t heap_addr_t;

typedef enum CeleryHeapConstants {
    HEAP_BODY,
    HEAP_NEXT,
    HEAP_LINK,
    HEAP_PARENT,
    HEAP_KIND,
} celery_heap_constant_t;

typedef enum CeleryHeapEntryType {
    CSH_STRING,
    CSH_BOOL,
    CSH_NUMBER,
    CSH_ADDRESS,
} celery_heap_entry_type_t;

typedef union CeleryHeapEntryValue {
    double number_value;
    bool bool_value;
    char* str_value;
} celery_heap_entry_value_t;

typedef struct CeleryHeapEntryKV {
    heap_key_t key;
    celery_heap_entry_type_t type;
    celery_heap_entry_value_t value;
} celery_heap_entry_kv_t;

typedef struct CeleryHeapEntry {
    size_t number_entries;
    celery_heap_entry_kv_t** kvs;
    heap_key_t kind;
} celery_heap_entry_t;

typedef struct CeleryHeap {
    heap_addr_t here;
    size_t heap_size;
    celery_heap_entry_t** entries;
} celery_heap_t;

celery_heap_t* heap_init();
void heap_alot(celery_heap_t* heap, heap_key_t kind);
void heap_put(celery_heap_t* heap, heap_addr_t adr, celery_heap_entry_type_t type, heap_key_t key, celery_heap_entry_value_t value);
void heap_set_kind(celery_heap_t* heap, heap_addr_t addr, heap_key_t kind);
#endif
