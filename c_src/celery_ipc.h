#ifndef CELERY_IPC_H
#define CELECY_IPC_H

#include <stdint.h>

#define NAMESPACE_SIZE 4
#define OPERATION_SIZE 5

typedef enum CeleryIPCType {
  CELERY_IPC_REQUEST,
  CELERY_IPC_RESPONSE
} celery_ipc_type_t;

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

typedef union CeleryIPCValue {
  celery_ipc_request_t* request;
  celery_ipc_response_t* response;
} celery_ipc_value_t;

typedef struct CeleryIPC {
  celery_ipc_type_t type;
  celery_ipc_value_t value;
} celery_ipc_t;

char* ipc_request_encode(celery_ipc_request_t* request, size_t* payload_size);
celery_ipc_request_t* ipc_request_decode_header(char* header);

celery_ipc_response_t* ipc_response_decode(char* response);
char* ipc_response_encode(celery_ipc_response_t* response, size_t *size);

int ipc_request_header_size();
char nth_byte(uint16_t number, size_t n);

#endif
