#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h> // ntohs
#include <stdio.h>

#include "celery_ipc.h"

char* ipc_request_encode(celery_ipc_request_t* request, size_t* size_ptr) {
    int size =
        sizeof(uint16_t) // Channel.
        + (sizeof(char) * NAMESPACE_SIZE) // Namespace.
        + (sizeof(char) * OPERATION_SIZE) // Command/Operation.
        + sizeof(uint16_t) // payload size.
        + (sizeof(char) * 2) // newline char
        + request->payload_size; // payload.

    char* ret = malloc(size);
    *size_ptr = size;

    size_t channel_number_offset = 0;
    size_t namespace_offset = sizeof(uint16_t);
    size_t operation_offset = namespace_offset + (sizeof(char) * NAMESPACE_SIZE);
    size_t payload_size_offset = operation_offset + (sizeof(char) * OPERATION_SIZE);
    size_t payload_offset = payload_size_offset + sizeof(uint16_t) + sizeof(char) * 2;

    ret[channel_number_offset] = nth_byte(request->channel_number, 1);
    ret[channel_number_offset + 1] = nth_byte(request->channel_number, 0);
    memcpy(&ret[namespace_offset], request->namespace, NAMESPACE_SIZE);
    memcpy(&ret[operation_offset], request->operation, OPERATION_SIZE);
    ret[payload_size_offset] = nth_byte(request->payload_size, 1);
    ret[payload_size_offset + 1] = nth_byte(request->payload_size, 0);
    ret[payload_size_offset + 2] = '\r';
    ret[payload_size_offset + 3] = '\n';
    memcpy(&ret[payload_offset], request->payload, request->payload_size);

    return ret;
}

celery_ipc_request_t* decode_header(char* header) {
  celery_ipc_request_t*  req = malloc(sizeof(celery_ipc_request_t));

  size_t channel_number_offset = 0;
  size_t namespace_offset = sizeof(uint16_t);
  size_t operation_offset = namespace_offset + (sizeof(char) * NAMESPACE_SIZE);
  size_t payload_size_offset = operation_offset + (sizeof(char) * OPERATION_SIZE);

  unsigned int tmp;
  tmp = (unsigned)header[channel_number_offset] << 8 | (unsigned)header[channel_number_offset + 1];
  req->channel_number = tmp;
  fprintf(stderr, "read: %u\n", tmp);

  // memcpy(req->namespace, &header[namespace_offset], NAMESPACE_SIZE);
  // req->namespace[NAMESPACE_SIZE + 1] = '\0';
  //
  // memcpy(req->operation, &header[operation_offset], OPERATION_SIZE);
  // req->operation[OPERATION_SIZE + 1] = '\0';
  //
  // tmp = (unsigned)header[payload_size_offset] << 8 | (unsigned)header[payload_size_offset + 1];
  // req->payload_size = tmp;
  //
  // req->payload = malloc(req->payload_size);

  return req;
}

celery_ipc_response_t* ipc_response_decode(char* response) {
    celery_ipc_response_t* resp = malloc(sizeof(celery_ipc_response_t));
    unsigned int tmp;
    tmp = (unsigned)response[0] << 8 | (unsigned)response[1];
    resp->channel_number = tmp;

    tmp = (unsigned)response[2] << 8 | (unsigned)response[3];
    resp->return_code = tmp;

    tmp = (unsigned)response[4] << 8 | (unsigned)response[5];
    resp->return_value = tmp;
    return resp;
}

char nth_byte(uint16_t number, size_t n) {
    return (number >> (8*n)) & 0xff;
}
