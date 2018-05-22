#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// For sighangup handling.
#include <signal.h>
#include <sys/prctl.h>
#include <linux/prctl.h>

#include <termios.h>

#include "csvm.h"
#include "celery_ipc.h"
#include "util.h"

// Testing
#include <string.h>
#include "celery_script.h"
#include "celery_slicer.h"

/* Initialize new terminal i/o settings */
void init_termios() {
    struct termios settings;
    tcgetattr(0, &settings); /* grab old terminal i/o settings */
    settings.c_lflag &= ~ICANON; /* disable buffered i/o */
    settings.c_lflag &= ~ECHO; /* set no echo mode */
    tcsetattr(0, TCSANOW, &settings); /* use these new terminal i/o settings now */
}

celery_ipc_request_t* get_request() {
    char request_header_buffer[ipc_request_header_size()];
    size_t total_bytes_read = 0;
    char read;

    while(total_bytes_read != ipc_request_header_size()) {
        read = getchar();
        request_header_buffer[total_bytes_read] = read;
        total_bytes_read += 1;
    }

    debug_print("\r\n");
    celery_ipc_request_t* request = ipc_request_decode_header(&request_header_buffer[0]);
    debug_print("\tGot request packet. \r\n");
    debug_print("\t\tchannel_number: %u \r\n", (unsigned int)request->channel_number);
    debug_print("\t\tnamespace: %s\r\n", request->namespace);
    debug_print("\t\toperation: %s\r\n", request->operation);
    debug_print("\t\tpayload_size: %u \r\n", (unsigned int)request->payload_size);
    debug_print("\r\tok\r\n\n");
    total_bytes_read = 0;

    while(total_bytes_read != request->payload_size) {
        request->payload[total_bytes_read] = getchar();
        total_bytes_read += 1;
    }

    return request;
}

celery_ipc_response_t* process_request(celery_ipc_request_t* request) {
    debug_print("\r\n\r\nStart procesing.\r\n\r\n");

    celery_ipc_response_t* resp = malloc(sizeof(celery_ipc_response_t));
    resp->channel_number = request->channel_number;
    resp->return_code = 150;
    resp->return_value = 166;
    //just for a test
    if(strcmp(request->namespace, "CODE") == 0) {
      debug_print("OK\r\n");
      celery_script_t* cs = buffer_to_celery_script(request->payload);
      celery_heap_t* heap = slice(cs);
    } else {
      debug_print("KO: %s\r\n", request->namespace);
    }

    debug_print("\r\n\r\nEnd procesing.\r\n\r\n");
    return resp;
}

void write_response(celery_ipc_response_t* response) {
    size_t size;
    char* packet = ipc_response_encode(response, &size);
    fwrite(packet, sizeof(char), size, stdout);
    fflush(stdout);
    free(packet);
}

void *fde(void *arg) {
  celery_ipc_request_t* req;
  celery_ipc_response_t* resp;
  for(;;) {
      debug_print("Waiting for new request \r\n");
      req = get_request();

      debug_print("processing request. \r\n");
      resp = process_request(req);
      free(req->payload);
      free(req);

      debug_print("writing response\r\n");
      write_response(resp);
      free(resp);
  }
}

int main(int argc, char const *argv[]) {
    // Handle sighangup when the port closes
    prctl(PR_SET_PDEATHSIG, SIGHUP);
    init_termios();

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, fde, NULL);
    pthread_join(thread_id, NULL);

    return 300;
}
