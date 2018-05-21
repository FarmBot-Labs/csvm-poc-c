#include "csvm.h"

// #include <ei.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON/cJSON.h"

#include "celery_heap.h"
#include "celery_slicer.h"

int main(int argc, char const *argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Not enough args %d\r\n", argc);
        return -1;
    }

    FILE *f = fopen(argv[1], "rb");
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
