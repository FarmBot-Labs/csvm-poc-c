#include "csvm.h"

#include <ei.h>
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

celery_script_t* json_to_celery_script(cJSON* celery_script_json) {
  celery_script_t* root;
  int i, body_size, args_size;

  cJSON* kind_string_json = cJSON_GetObjectItem(celery_script_json, "kind");
  cJSON* body_array_json = cJSON_GetObjectItem(celery_script_json, "body");
  cJSON* args_object_json = cJSON_GetObjectItem(celery_script_json, "args");

  body_size = cJSON_GetArraySize(body_array_json);
  celery_script_t* body_array[body_size];
  for(i = 0; i < body_size; i++) {
    body_array[i] = json_to_celery_script(cJSON_GetArrayItem(body_array_json, i));
  }

  args_size = cJSON_GetArraySize(args_object_json);
  celery_arg_t* args_array[args_size];
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
  memcpy(&root->body, &body_array, body_size);
  memcpy(&root->args, &args_array, args_size);
  printf("Celery item: %s with %d nodes in body\r\n", root->kind, body_size);
  return root;
}

int main(int argc, char const *argv[]) {
  if(argc != 2) {
    fprintf(stderr, "Not enough args %d\r\n", argc);
    return -1;
  }

  cJSON *json = cJSON_Parse(argv[1]);
  if(json == NULL) {
    fprintf(stderr, "Failed to parse json: %s\r\n", cJSON_GetErrorPtr());
    return -1;
  }

  celery_script_t* celery = json_to_celery_script(json);

  /* code */
  return 0;
}
