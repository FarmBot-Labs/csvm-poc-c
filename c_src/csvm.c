#include "csvm.h"

// #include <ei.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "cJSON/cJSON.h"

#include "celery_heap.h"
#include "celery_slicer.h"
#include "celery_ipc.h"

int main(int argc, char const *argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Not enough args %d\r\n", argc);
        return -1;
    }

    char* to_decode = malloc(8);
    memcpy(to_decode, argv[1], 8);
    for(int i = 0; i<8; i = i+2 ) {
        fprintf(stderr, "[0x%02X 0x%02X] ", argv[1][i], argv[1][i+1]);
    }
    fprintf(stderr, "\r\n");

    celery_ipc_response_t* resp = ipc_response_decode(to_decode);
    fprintf(stderr, "Got response: channel: %" PRIu16, resp->channel_number);
    fprintf(stderr, " 0x%02X ", resp->channel_number);
    fprintf(stderr, " return_code: %" PRIu16, resp->return_code);
    fprintf(stderr, " 0x%02X ", resp->return_code);
    fprintf(stderr, " return_value: %" PRIu16, resp->return_value);
    fprintf(stderr, " 0x%02X ", resp->return_value);
    fprintf(stderr, "\r\n");
    //
    // FILE *f = fopen(argv[1], "rb");
    // fseek(f, 0, SEEK_END);
    // long fsize = ftell(f);
    // fseek(f, 0, SEEK_SET);
    // char *string = malloc(fsize + 1);
    // fread(string, fsize, 1, f);
    // fclose(f);
    //
    // string[fsize] = 0;
    //
    // cJSON *json = cJSON_Parse(string);
    // if(json == NULL) {
    //     fprintf(stderr, "Failed to parse json: %s\r\n", cJSON_GetErrorPtr());
    //     return -1;
    // }

    // celery_script_t* celery = json_to_celery_script(json);
    // celery_heap_t* heap = slice(celery);
    // inspect_heap(heap);

    celery_ipc_request_t* ipc_request = malloc(sizeof(celery_ipc_request_t));
    ipc_request->channel_number = 20;
    char namespace[5] = {'a', 'b', 'c', 'd', '\0'};
    char operation[6] = {'e', 'f', 'g', 'h', 'i', '\0'};
    memcpy(ipc_request->namespace, namespace, 5);
    memcpy(ipc_request->operation, operation, 6);
    ipc_request->payload_size = 2;
    char payload[2] = {'j', 'k'};
    ipc_request->payload = malloc(2);
    memcpy(ipc_request->payload, payload, 2);

    size_t request_size;
    char* encoded = ipc_request_encode(ipc_request, &request_size);
    fwrite(encoded, sizeof(char), request_size, stdout);
    fflush(stdout);
    /* code */
    return 0;
}
