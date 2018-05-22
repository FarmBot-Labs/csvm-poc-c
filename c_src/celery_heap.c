#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "celery_heap.h"
#include "util.h"

celery_heap_t* heap_init() {
    celery_heap_t* heap = malloc(sizeof(celery_heap_t));
    heap->heap_size = 1;
    celery_heap_entry_t** heap_entries = malloc(sizeof(celery_heap_entry_t) * 1);
    if(!heap_entries) {
        debug_print("FAILED TO MALLOC ENTRIES\r\n");
    }
    heap_entries[0] = malloc(sizeof(celery_heap_entry_t));
    heap_entries[0]->number_entries = 0;

    heap->here = 0;
    heap->entries = heap_entries;
    int value = 0;
    heap_set_kind(heap, 0, "nothing");
    heap_put(heap, 0, CSH_STRING,  HEAP_KIND, "nothing");
    heap_put(heap, 0, CSH_ADDRESS, HEAP_PARENT, &value);
    heap_put(heap, 0, CSH_ADDRESS, HEAP_BODY, &value);
    heap_put(heap, 0, CSH_ADDRESS, HEAP_NEXT, &value);

    return heap;
}

void heap_alot(celery_heap_t* heap, char* kind) {
    debug_print("Current heap size: %d\r\n", heap->heap_size);
    // add a new entry on the heap.
    celery_heap_entry_t** new_entries = realloc(heap->entries, (heap->heap_size + 1) * sizeof(celery_heap_entry_t));
    if(!new_entries) {
        debug_print("FAILED TO REALOC %d\r\n", heap->heap_size);
        exit(1);
    }
    new_entries[heap->heap_size] = malloc(sizeof(celery_heap_entry_t));
    new_entries[heap->heap_size]->number_entries = 0;
    heap->entries = new_entries;
    heap->heap_size = heap->heap_size + 1;
    heap->here = heap->here + 1;
    heap_set_kind(heap, heap->here, kind);
    heap_put(heap, heap->here, CSH_STRING, HEAP_KIND, kind);
}

void heap_put(celery_heap_t* heap, int adr, celery_heap_entry_type_t type, char* key, void* value) {
    celery_heap_entry_t* entry = heap->entries[adr];
    if(!entry) {
        debug_print("NO ENTRY AT ADDRESS %d\r\n", adr);
        exit(1);
    }

    int i, found;
    for(i = 0, found = 0; (entry->number_entries > 0 && i<entry->number_entries) && (found == 0); i++) {
        found = strcmp(entry->kvs[i]->key, key) == 0;
    }

    if(found) {
        debug_print("updating key %s\r\n", key);
        entry->kvs[i-1]->key = key;
        entry->kvs[i-1]->type = type;
        entry->kvs[i-1]->value = value;
    } else if(entry->number_entries == 0) {
        debug_print("adding first key %s\r\n", key);
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

        /* ??????? */
        new_kvs[entry->number_entries] = malloc(sizeof(celery_heap_entry_kv_t));
        new_kvs[entry->number_entries]->key = key;
        new_kvs[entry->number_entries]->type = type;
        new_kvs[entry->number_entries]->value = value;
        entry->kvs = new_kvs;
        entry->number_entries = entry->number_entries + 1;
    }

}

void heap_set_kind(celery_heap_t* heap, int adr, char* kind) {
  celery_heap_entry_t* entry = heap->entries[adr];
  if(!entry) {
      debug_print("NO ENTRY AT ADDRESS %d\r\n", adr);
      exit(1);
  }
  int len = strlen(kind);
  entry->kind = malloc(len);
  memcpy(entry->kind, kind, len);
}

void inspect_heap(celery_heap_t* heap) {
    for(int i = 0; i<heap->heap_size; i++) {
        celery_heap_entry_t* entry = heap->entries[i];
        debug_print("ENTRY[%d] with %d number of entries.\r\n", i, entry->number_entries);
        for(int j = 0; j<entry->number_entries; j++) {
            celery_heap_entry_kv_t* kv = entry->kvs[j];
            char* key = kv->key;
            void* value = kv->value;

            switch(kv->type) {
            case CSH_STRING:
                debug_print("\t'%s' => '%s'\r\n", key, (char*)value);
                break;
            case CSH_BOOL:
            case CSH_NUMBER:
            case CSH_ADDRESS: {
                int actual_value = *(int*)value;
                debug_print("\t'%s' => %d\r\n", key, actual_value);
                break;
            }
            default:
                debug_print("\t'%s' => %p (unknown type)\r\n", key, value);
                break;
            }
        }
        debug_print("\r\n");
    }
}
