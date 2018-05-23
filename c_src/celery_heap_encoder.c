#include <stdlib.h>

#include "celery_heap.h"
#include "corpus.h"
#include "celery_script.h"

#include "cJSON/cJSON.h"

void celery_heap_encode(celery_heap_t* heap);

void celery_heap_encode(celery_heap_t* heap) {
  int i, j;
  cJSON* heap_json = cJSON_CreateObject();
  cJSON* heap_entries_json = cJSON_CreateArray();

  for(i=0;i<heap->heap_size;i++) {
    cJSON* heap_entry_json = cJSON_CreateObject();
    celery_heap_entry_t* entry = heap->entries[i];
    for(j=0; j<entry->number_entries;j++) {
      celery_heap_entry_kv_t* kv = entry->kvs[j];
      cJSON* value_json = NULL;
      cJSON* key_json = NULL;
      cJSON* type_json = NULL;

      switch(kv->type) {
        case CSH_STRING: {
          type_json = cJSON_CreateString("STRING");
          value_json = cJSON_CreateString(kv->value.str_value);
          key_json = cJSON_CreateString(string_from_node_name(kv->key));
          break;
        }
        case CSH_BOOL: {
          type_json = cJSON_CreateString("BOOL");
          value_json = cJSON_CreateBool(kv->value.bool_value);
          key_json = cJSON_CreateString(string_from_node_name(kv->key));
          break;
        }
        case CSH_NUMBER: {
          type_json = cJSON_CreateString("NUMBER");
          value_json = cJSON_CreateNumber(kv->value.number_value);
          key_json = cJSON_CreateString(string_from_node_name(kv->key));
          break;
        }
        case CSH_ADDRESS: {
          type_json = cJSON_CreateString("ADDRESS");
          value_json = cJSON_CreateNumber(kv->value.number_value);
          switch(kv->key) {
            case HEAP_BODY: {
              key_json = cJSON_CreateString("HEAP_BODY");
              break;
            }
            case HEAP_NEXT: {
              key_json = cJSON_CreateString("HEAP_NEXT");
              break;
            }
            case HEAP_LINK: {
              key_json = cJSON_CreateString("HEAP_LINK");
              break;
            }
            case HEAP_PARENT: {
              key_json = cJSON_CreateString("HEAP_PARENT");
              break;
            }
            case HEAP_KIND: {
              key_json = cJSON_CreateString("HEAP_KIND");
              break;
            }
            default: {
              key_json = cJSON_CreateString(string_from_arg_name(kv->key));
              break;
            }
          }
          break;
        }
      }
      // cJSON_AddItemToObject(heap_entry_json, )
    }
  }

  cJSON_AddItemToObject(heap_json, "entries", heap_entries_json);
}
