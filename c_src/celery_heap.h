#ifndef CELERYHEAP_H
#define CELERYHEAP_H

#define HEAP_BODY "HEAP_BODY"
#define HEAP_NEXT "HEAP_NEXT"
#define HEAP_LINK "HEAP_LINK"
#define HEAP_PARENT "HEAP_PARENT"
#define HEAP_KIND "HEAP_KIND"

typedef enum CeleryHeapEntryType {
    CSH_STRING,
    CSH_BOOL,
    CSH_NUMBER,
    CSH_ADDRESS,
    CSH_NULL
} celery_heap_entry_type_t;

typedef struct CeleryHeapEntryKV {
    char* key;
    void* value;
    celery_heap_entry_type_t type;
} celery_heap_entry_kv_t;

typedef struct CeleryHeapEntry {
    int number_entries;
    celery_heap_entry_kv_t** kvs;
    char* kind;
} celery_heap_entry_t;

typedef struct CeleryHeap {
    int here;
    int heap_size;
    celery_heap_entry_t** entries;
} celery_heap_t;

celery_heap_t* heap_init();
void heap_alot(celery_heap_t* heap, char* kind);
void heap_put(celery_heap_t* heap, int adr, celery_heap_entry_type_t type, char* key, void* value);
void heap_set_kind(celery_heap_t* heap, int addr, char* kind);
void inspect_heap(celery_heap_t* heap);
#endif
