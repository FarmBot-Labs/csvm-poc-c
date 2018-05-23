#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "celery_heap.h"
#include "corpus.h"
#include "util.h"

celery_heap_t* heap_init() {
    celery_heap_t* heap = malloc(sizeof(celery_heap_t));
    heap->heap_size = 1;
    celery_heap_entry_t** heap_entries = malloc(sizeof(celery_heap_entry_t));
    if(!heap_entries) {
        debug_print("FAILED TO MALLOC ENTRIES\r\n");
    }
    heap_entries[0] = malloc(sizeof(celery_heap_entry_t));
    heap_entries[0]->number_entries = 0;

    heap->here = 0;
    heap->entries = heap_entries;
    celery_heap_entry_value_t value;
    value.number_value = 0;
    heap_set_kind(heap, 0, NOTHING);
    heap_put(heap, 0, CSH_ADDRESS, HEAP_PARENT, value);
    heap_put(heap, 0, CSH_ADDRESS, HEAP_BODY, value);
    heap_put(heap, 0, CSH_ADDRESS, HEAP_NEXT, value);

    return heap;
}

void heap_alot(celery_heap_t* heap, heap_key_t kind) {
    debug_print("Current heap size: %ld\r\n", heap->heap_size);
    // add a new entry on the heap.
    celery_heap_entry_t** new_entries = realloc(heap->entries, (heap->heap_size + 1) * sizeof(celery_heap_entry_t));
    if(!new_entries) {
        debug_print("FAILED TO REALOC %ld\r\n", heap->heap_size);
        exit(1);
    }
    new_entries[heap->heap_size] = malloc(sizeof(celery_heap_entry_t));
    new_entries[heap->heap_size]->number_entries = 0;
    heap->entries = new_entries;
    heap->heap_size = heap->heap_size + 1;
    heap->here = heap->here + 1;
    heap_set_kind(heap, heap->here, kind);
}

void heap_put(celery_heap_t* heap, heap_addr_t adr, celery_heap_entry_type_t type, heap_key_t key, celery_heap_entry_value_t value) {
    celery_heap_entry_t* entry = heap->entries[adr];
    if(!entry) {
        debug_print("NO ENTRY AT ADDRESS %ld\r\n", adr);
        exit(1);
    }

    int i, found;
    for(i = 0, found = 0; (entry->number_entries > 0 && i<entry->number_entries) && (found == 0); i++) {
        found = entry->kvs[i]->key == key;
    }

    if(found) {
        debug_print("updating key %s\r\n", key);
        entry->kvs[i-1]->key = key;
        entry->kvs[i-1]->type = type;
        entry->kvs[i-1]->value = value;
    } else if(entry->number_entries == 0) {
        debug_print("adding first key %ls\r\n", key);
        entry->kvs = malloc(1 * sizeof(celery_heap_entry_kv_t));
        entry->kvs[0] = malloc(sizeof(celery_heap_entry_kv_t));
        entry->kvs[0]->key = key;
        entry->kvs[0]->type = type;
        entry->kvs[0]->value = value;
        entry->number_entries = 1;
    } else {
        debug_print("adding key %s (currently %d entries)\r\n", key, entry->number_entries);

        celery_heap_entry_kv_t** new_kvs = realloc(entry->kvs, (entry->number_entries + 1) * sizeof(celery_heap_entry_kv_t));
        if(!new_kvs) {
            debug_print("Failed to realloc new kv entries\r\n");
            exit(1);
        }

        new_kvs[entry->number_entries] = malloc(sizeof(celery_heap_entry_kv_t));
        new_kvs[entry->number_entries]->key = key;
        new_kvs[entry->number_entries]->type = type;
        new_kvs[entry->number_entries]->value = value;
        entry->kvs = new_kvs;
        entry->number_entries = entry->number_entries + 1;
    }

}

void heap_set_kind(celery_heap_t* heap, heap_addr_t adr, heap_key_t kind) {
    celery_heap_entry_t* entry = heap->entries[adr];
    if(!entry) {
        debug_print("NO ENTRY AT ADDRESS %d\r\n", adr);
        exit(1);
    }
    entry->kind = kind;
}
