#ifndef CELERY_IPC_H
#define CELECY_IPC_H

#include <stdint.h>

#define NAMESPACE_SIZE 4
#define OPERATION_SIZE 5

typedef struct CeleryIPCRequest {
    uint16_t channel_number;
    char namespace[NAMESPACE_SIZE + 1]; // Add char for null byte.
    char operation[OPERATION_SIZE + 1]; // Add char for null byte.
    uint16_t payload_size;
    char* payload;
} celery_ipc_request_t;

typedef struct CeleryIPCResponse {
    uint16_t channel_number;
    uint16_t return_code;
    uint16_t return_value;
} celery_ipc_response_t;

char* ipc_request_encode(celery_ipc_request_t* request, size_t* payload_size);
celery_ipc_response_t* ipc_response_decode(char* response);

char nth_byte(uint16_t number, size_t n);

#endif
