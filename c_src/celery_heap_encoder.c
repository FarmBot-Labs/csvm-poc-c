#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "celery_heap.h"
#include "corpus.h"
#include "celery_script.h"
#include "util.h"

#include "cJSON/cJSON.h"

#include "celery_heap_encoder.h"

void celery_heap_encode_json(celery_heap_t* heap) {
  debug_print("Encoding heap of size: %ld\r\n", heap->heap_size);
  int i, j;
  cJSON* heap_json = cJSON_CreateObject();
  cJSON* heap_entries_json = cJSON_CreateArray();

  for(i=0;i<heap->heap_size;i++) {
    celery_heap_entry_t* entry = heap->entries[i];
    debug_print("Encoding heap entry of size: %ld\r\n", entry->number_entries);

    cJSON* heap_entry_json = cJSON_CreateObject();
    cJSON* kind_json = cJSON_CreateString(string_from_node_name(entry->kind));

    for(j=0; j<entry->number_entries;j++) {
      celery_heap_entry_kv_t* kv = entry->kvs[j];
      debug_print("Encoding entry kv\r\n");
      cJSON* kv_json = cJSON_CreateObject();
      cJSON* value_json = NULL;
      cJSON* key_json = NULL;
      cJSON* type_json = NULL;

      switch(kv->type) {
        case CSH_STRING: {
          type_json = cJSON_CreateString("STRING");
          value_json = cJSON_CreateString(kv->value.str_value);
          key_json = cJSON_CreateString(string_from_arg_name(kv->key));
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

      debug_print("Adding key %s to kv_json\r\n", key_json->valuestring);
      cJSON_AddItemToObject(kv_json, "key", key_json);

      debug_print("Adding value %s to kv_json\r\n", value_json->valuestring);
      cJSON_AddItemToObject(kv_json, "value", value_json);

      debug_print("Adding type %s to kv_json\r\n", type_json->valuestring);
      cJSON_AddItemToObject(kv_json, "type", type_json);

      debug_print("Adding kv to heap_entries\r\n");
      cJSON_AddItemToArray(heap_entries_json, kv_json);
    }

    debug_print("Adding kind %s to heap_entry\r\n", kind_json->valuestring);
    cJSON_AddItemToObject(heap_entry_json, "kind", kind_json);
  }

  cJSON_AddItemToObject(heap_json, "entries", heap_entries_json);
  fprintf(stderr, cJSON_Print(heap_json));
}
