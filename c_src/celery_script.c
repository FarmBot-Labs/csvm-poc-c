#include <stdlib.h>

#include "cJSON/cJSON.h"

#include "celery_script.h"

celery_script_t* json_to_celery_script(char* data) {
    celery_script_t* root;
    int i, body_size, args_size;
    cJSON* celery_script_json = cJSON_Parse(data);

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
    cJSON_free(celery_script_json);
    return root;
}
