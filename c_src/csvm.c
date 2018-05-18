#include "csvm.h"

// #include <ei.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

typedef enum CeleryArgType {
  CS_STRING,
  CS_BOOL,
  CS_NUMBER,
  CS_CELERYSCRIPT
} celery_arg_type_t;

typedef enum CeleryHeapEntryType {
  CSH_STRING,
  CSH_BOOL,
  CSH_NUMBER,
  CSH_ADDRESS,
  CSH_NULL
} celery_heap_entry_type_t;

typedef struct CeleryArg {
  char* kind;
  void* value;
  celery_arg_type_t type;
} celery_arg_t;

typedef struct CeleryScript {
  char* kind;
  int body_size;
  int args_size;
  struct CeleryScript** body;
  struct CeleryArg** args;
} celery_script_t;

typedef struct CeleryHeapEntryKV {
  char* key;
  void* value;
  celery_heap_entry_type_t type;
} celery_heap_entry_kv_t;

typedef struct CeleryHeapEntry {
  int number_entries;
  celery_heap_entry_kv_t** kvs;
} celery_heap_entry_t;

typedef struct CeleryHeap {
  int here;
  int heap_size;
  celery_heap_entry_t** entries;
} celery_heap_t;

char* HEAP_BODY = "HEAP_BODY";
char* HEAP_NEXT = "HEAP_NEXT";
char* HEAP_LINK = "HEAP_LINK";
char* HEAP_PARENT = "HEAP_PARENT";
char* HEAP_KIND = "HEAP_KIND";
// Function declerations.

celery_script_t* json_to_celery_script(cJSON* celery_script_json);

celery_heap_t* slice(celery_script_t* celery);
int slicer_allocate(celery_heap_t* heap, celery_script_t* celery, int addr);
void slicer_iterate_over_body(celery_heap_t* heap, celery_script_t** body, int body_size, int prev_addr);
void slicer_iterate_over_args(celery_heap_t* heap, celery_arg_t** args, int args_size, int addr);

celery_heap_t* heap_init();
void heap_alot(celery_heap_t* heap, char* kind);
void heap_put(celery_heap_t* heap, int adr, celery_heap_entry_type_t type, char* key, void* value);


celery_script_t* json_to_celery_script(cJSON* celery_script_json) {
  celery_script_t* root;
  int i, body_size, args_size;

  cJSON* kind_string_json = cJSON_GetObjectItem(celery_script_json, "kind");
  cJSON* body_array_json = cJSON_GetObjectItem(celery_script_json, "body");
  cJSON* args_object_json = cJSON_GetObjectItem(celery_script_json, "args");

  body_size = cJSON_GetArraySize(body_array_json);
  // celery_script_t* body_array[body_size];
  celery_script_t** body_array = malloc(sizeof(celery_script_t) * body_size);
  for(i = 0; i < body_size; i++) {
    body_array[i] = json_to_celery_script(cJSON_GetArrayItem(body_array_json, i));
  }

  args_size = cJSON_GetArraySize(args_object_json);
  celery_arg_t** args_array = malloc(sizeof(celery_arg_t) * args_size);
  for(i = 0; i < args_size; i++) {
    celery_arg_t* arg = malloc(sizeof(celery_arg_t));
    cJSON* itm = cJSON_GetArrayItem(args_object_json, i);
    arg->kind = itm->string;
    if(cJSON_IsBool(itm)) {
      arg->type = CS_BOOL;
      arg->value = &itm->valueint;
    } else if(cJSON_IsNumber(itm)) {
      arg->type = CS_NUMBER;
      arg->value = &itm->valuedouble;
    } else if(cJSON_IsString(itm)) {
      arg->type = CS_STRING;
      arg->value = cJSON_GetStringValue(itm);
    } else if(cJSON_IsObject(itm)) {
      arg->type = CS_CELERYSCRIPT;
      arg->value = json_to_celery_script(itm);
    }
    args_array[i] = arg;
  }

  root = malloc(sizeof(celery_script_t));
  root->kind = cJSON_GetStringValue(kind_string_json);
  root->body_size = body_size;
  root->args_size = args_size;
  root->body = body_array;
  root->args = args_array;
  // printf("Celery item: %s with %d nodes in body\r\n", root->kind, body_size);
  return root;
}

celery_heap_t* slice(celery_script_t* celery) {
  celery_heap_t* heap = heap_init();
  int addr = slicer_allocate(heap, celery, 0);
  for(int i=0; i<heap->heap_size; i++) {
    // Update each entry to have a body address, and a next address.
  }
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
      char* buf;
      snprintf(buf, strlen(HEAP_LINK) + 1 + strlen(args[i]->kind) + 1, "%s_%s", HEAP_LINK, args[i]->kind);
      int addr = slicer_allocate(heap, args[i]->value, parent_addr);
      heap_put(heap, parent_addr, CSH_ADDRESS, buf, &addr);
    } else {
      heap_put(heap, parent_addr, args[i]->type, args[i]->kind, args[i]->value);
    }
  }
}

celery_heap_t* heap_init() {
  celery_heap_t* heap = malloc(sizeof(celery_heap_t));
  heap->heap_size = 1;
  celery_heap_entry_t** heap_entries = malloc(sizeof(celery_heap_entry_t) * 1);
  if(!heap_entries) {
    fprintf(stderr, "FAILED TO MALLOC ENTRIES\r\n");
  }
  heap_entries[0] = malloc(sizeof(celery_heap_entry_t));
  heap_entries[0]->number_entries = 0;

  heap->here = 0;
  heap->entries = heap_entries;
  int value = 0;
  heap_put(heap, 0, CSH_STRING,  HEAP_KIND, "nothing");
  heap_put(heap, 0, CSH_ADDRESS, HEAP_PARENT, &value);
  heap_put(heap, 0, CSH_ADDRESS, HEAP_BODY, &value);
  heap_put(heap, 0, CSH_ADDRESS, HEAP_NEXT, &value);

  return heap;
}

void heap_alot(celery_heap_t* heap, char* kind) {
  fprintf(stderr, "Current heap size: %d\r\n", heap->heap_size);
  // add a new entry on the heap.
  celery_heap_entry_t** new_entries = realloc(heap->entries, (heap->heap_size + 1) * sizeof(celery_heap_entry_t));
  if(!new_entries) {
    fprintf(stderr, "FAILED TO REALOC %d\r\n", heap->heap_size);
    exit(1);
  }
  new_entries[heap->heap_size] = malloc(sizeof(celery_heap_entry_t));
  new_entries[heap->heap_size]->number_entries = 0;
  heap->entries = new_entries;
  heap->heap_size = heap->heap_size + 1;
  heap->here = heap->here + 1;
  heap_put(heap, heap->here, CSH_STRING, HEAP_KIND, kind);
}

void heap_put(celery_heap_t* heap, int adr, celery_heap_entry_type_t type, char* key, void* value) {
  // switch(type) {
  //   case CSH_STRING:
  //     fprintf(stderr, "putting key: '%s' => '%s' on heap at address %d\r\n", key, value, adr);
  //     break;
  //   case CSH_BOOL:
  //   case CSH_NUMBER:
  //   case CSH_ADDRESS: {
  //     int actual_value;
  //     actual_value = *((int*) value);
  //     fprintf(stderr, "putting key: '%s' => %d on heap at address %d\r\n", key, actual_value, adr);
  //     break;
  //   }
  //
  //   case CSH_NULL:
  //     fprintf(stderr, "putting key: '%s' => [NULL] on heap at address %d\r\n", key, adr);
  //     break;
  //   default:
  //     fprintf(stderr, "putting key: '%s' => %p on heap at address %d\r\n", key, value, adr);
  //     break;
  // }

  celery_heap_entry_t* entry = heap->entries[adr];
  if(!entry) {
    fprintf(stderr, "NO ENTRY AT ADDRESS %d\r\n", adr);
    exit(1);
  }

  int i, found;
  for(i = 0, found = 0; (entry->number_entries > 0 && i<entry->number_entries) && (found == 0); i++) {
    found = strcmp(entry->kvs[i]->key, key) == 0;
  }

  if(found) {
    printf("updating key %s\r\n", key);
    entry->kvs[i-1]->key = key;
    entry->kvs[i-1]->type = type;
    entry->kvs[i-1]->value = value;
  } else if(entry->number_entries == 0) {
    printf("adding first key %s\r\n", key);
    entry->kvs = malloc(1 * sizeof(celery_heap_entry_kv_t));
    entry->kvs[0] = malloc(sizeof(celery_heap_entry_kv_t));
    entry->kvs[0]->key = key;
    entry->kvs[0]->type = type;
    entry->kvs[0]->value = value;
    entry->number_entries = 1;
  } else {
    printf("adding key %s (currently %d entries)\r\n", key, entry->number_entries);

    celery_heap_entry_kv_t** new_kvs = realloc(entry->kvs, (entry->number_entries + 1) * sizeof(celery_heap_entry_kv_t));
    if(!new_kvs) {
      fprintf(stderr, "Failed to realloc new kv entries\r\n");
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

void inspect_heap(celery_heap_t* heap) {
  for(int i = 0; i<heap->heap_size; i++) {
    celery_heap_entry_t* entry = heap->entries[i];
    printf("ENTRY[%d] with %d number of entries.\r\n", i, entry->number_entries);
    for(int j = 0; j<entry->number_entries; j++) {
      celery_heap_entry_kv_t* kv = entry->kvs[j];
      char* key = entry->kvs[j]->key;
      void* value = entry->kvs[j]->value;

      switch(entry->kvs[j]->type) {
        case CSH_STRING:
          printf("\t'%s' => '%s'\r\n", key, value);
          break;
        case CSH_BOOL:
        case CSH_NUMBER:
        case CSH_ADDRESS: {
          int actual_value = *(int*)value;
          // actual_value = *((int*) value);
          printf("\t'%s' => %d\r\n", key, actual_value);
          break;
        }
        default:
          printf("\t'%s' => %p (unknown type)\r\n", key, value);
          break;
      }
    }
    printf("\r\n");
  }
}

int main(int argc, char const *argv[]) {
  // if(argc != 2) {
  //   fprintf(stderr, "Not enough args %d\r\n", argc);
  //   return -1;
  // }

  FILE *f = fopen("sequence.json", "rb");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *string = malloc(fsize + 1);
  fread(string, fsize, 1, f);
  fclose(f);

  string[fsize] = 0;

  cJSON *json = cJSON_Parse(string);
  if(json == NULL) {
    fprintf(stderr, "Failed to parse json: %s\r\n", cJSON_GetErrorPtr());
    return -1;
  }

  celery_script_t* celery = json_to_celery_script(json);
  celery_heap_t* heap = slice(celery);
  inspect_heap(heap);

  /* code */
  return 0;
}
