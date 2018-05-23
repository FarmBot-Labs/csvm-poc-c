#include <stdlib.h>

#include "cJSON/cJSON.h"

#include "celery_script.h"
#include "corpus.h"

celery_script_t* buffer_to_celery_script(char* buffer) {
    cJSON* json = cJSON_Parse(buffer);
    celery_script_t* cs = json_to_celery_script(json);
    cJSON_free(json);
    return cs;
}

celery_script_t* json_to_celery_script(cJSON* celery_script_json) {
    celery_script_t* root;
    int i, body_size, args_size;

    cJSON* kind_string_json = cJSON_GetObjectItem(celery_script_json, "kind");
    cJSON* body_array_json = cJSON_GetObjectItem(celery_script_json, "body");
    cJSON* args_object_json = cJSON_GetObjectItem(celery_script_json, "args");

    body_size = cJSON_GetArraySize(body_array_json);
    celery_script_t** body_array = malloc(sizeof(celery_script_t) * body_size);
    for(i = 0; i < body_size; i++) {
        body_array[i] = json_to_celery_script(cJSON_GetArrayItem(body_array_json, i));
    }

    args_size = cJSON_GetArraySize(args_object_json);
    celery_arg_t** args_array = malloc(sizeof(celery_arg_t) * args_size);
    for(i = 0; i < args_size; i++) {
        celery_arg_t* arg = malloc(sizeof(celery_arg_t));
        celery_arg_value_t arg_value;
        cJSON* itm = cJSON_GetArrayItem(args_object_json, i);
        arg->kind = arg_name_from_string(itm->string);
        if(cJSON_IsBool(itm)) {
            arg->type = CS_BOOL;
            arg_value.bool_value = (bool)itm->valueint;
        } else if(cJSON_IsNumber(itm)) {
            arg->type = CS_NUMBER;
            arg_value.number_value = itm->valuedouble;
        } else if(cJSON_IsString(itm)) {
            arg->type = CS_STRING;
            char* value_str = cJSON_GetStringValue(itm);
            arg_value.str_value = value_str;
        } else if(cJSON_IsObject(itm)) {
            arg->type = CS_CELERYSCRIPT;
            celery_script_t* celery_value = json_to_celery_script(itm);
            arg_value.celery_value = celery_value;
        }
        arg->value = arg_value;
        args_array[i] = arg;
    }

    root = malloc(sizeof(celery_script_t));
    root->kind = node_name_from_string(cJSON_GetStringValue(kind_string_json));
    root->body_size = body_size;
    root->args_size = args_size;
    root->body = body_array;
    root->args = args_array;
    return root;
}
