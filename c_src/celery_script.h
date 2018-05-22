#ifndef CELERYSCRIPT_H
#define CELERYSCRIPT_H

#include "cJSON/cJSON.h"

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

celery_script_t* buffer_to_celery_script(char* buffer);
celery_script_t* json_to_celery_script(cJSON* celery_script_json);

#endif
