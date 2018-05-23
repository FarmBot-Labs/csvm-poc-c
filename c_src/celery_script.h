#ifndef CELERYSCRIPT_H
#define CELERYSCRIPT_H

#include <stdbool.h>
#include "cJSON/cJSON.h"
#include "corpus.h"

typedef enum CeleryArgType {
    CS_STRING,
    CS_BOOL,
    CS_NUMBER,
    CS_CELERYSCRIPT
} celery_arg_type_t;

typedef union CeleryArgValue {
    char* str_value;
    bool bool_value;
    double number_value;
    struct CeleryScript* celery_value;
} celery_arg_value_t;

typedef struct CeleryArg {
    celery_node_kind_name_t kind;
    celery_arg_type_t type;
    celery_arg_value_t value;
} celery_arg_t;

typedef struct CeleryScript {
    celery_node_kind_name_t kind;
    size_t body_size;
    size_t args_size;
    struct CeleryScript** body;
    struct CeleryArg** args;
} celery_script_t;

celery_script_t* buffer_to_celery_script(char* buffer);
celery_script_t* json_to_celery_script(cJSON* celery_script_json);

#endif
