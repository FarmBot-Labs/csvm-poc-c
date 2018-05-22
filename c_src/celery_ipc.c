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
    if(size_ptr) {
      *size_ptr = size;
    }

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

  memcpy(&req->channel_number, &header[channel_number_offset], sizeof(int16_t));
  req->channel_number = (int16_t)ntohs((uint16_t)req->channel_number);

  memcpy(req->namespace, &header[namespace_offset], NAMESPACE_SIZE);
  req->namespace[NAMESPACE_SIZE + 1] = '\0';

  memcpy(req->operation, &header[operation_offset], OPERATION_SIZE);
  req->operation[OPERATION_SIZE + 1] = '\0';

  memcpy(&req->payload_size, &header[payload_size_offset], sizeof(int16_t));
  req->payload_size = (int16_t)ntohs((uint16_t)req->payload_size);

  req->payload = malloc(req->payload_size);

  return req;
}

celery_ipc_response_t* ipc_response_decode(char* response) {
    celery_ipc_response_t* resp = malloc(sizeof(celery_ipc_response_t));

    memcpy(&resp->channel_number, &response[0], sizeof(int16_t));
    resp->channel_number = (int16_t)ntohs((uint16_t)resp->channel_number);

    memcpy(&resp->return_code, &response[2], sizeof(int16_t));
    resp->return_code = (int16_t)ntohs((uint16_t)resp->return_code);

    memcpy(&resp->return_value, &response[4], sizeof(int16_t));
    resp->return_value = (int16_t)ntohs((uint16_t)resp->return_value);

    return resp;
}

char nth_byte(uint16_t number, size_t n) {
    return (number >> (8*n)) & 0xff;
}
