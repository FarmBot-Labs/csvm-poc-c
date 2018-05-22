#include <termios.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <linux/prctl.h>
#include <string.h>
#include <arpa/inet.h> // ntohs

#include "csvm.h"
#include "celery_ipc.h"

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

    fprintf(stderr, "\r\n");
    celery_ipc_request_t* request = ipc_request_decode_header(&request_header_buffer[0]);
    fprintf(stderr, "\tGot request packet. \r\n");
    fprintf(stderr, "\t\tchannel_number: %u \r\n", (unsigned int)request->channel_number);
    fprintf(stderr, "\t\tnamespace: %s\r\n", request->namespace);
    fprintf(stderr, "\t\toperation: %s\r\n", request->operation);
    fprintf(stderr, "\t\tpayload_size: %u \r\n", (unsigned int)request->payload_size);
    fprintf(stderr, "\r\tok\r\n\n");
    total_bytes_read = 0;

    while(total_bytes_read != request->payload_size) {
        request->payload[total_bytes_read] = getchar();
        total_bytes_read += 1;
    }

    return request;
}

celery_ipc_response_t* process_request(celery_ipc_request_t* request) {
    celery_ipc_response_t* resp = malloc(sizeof(celery_ipc_response_t));
    resp->channel_number = request->channel_number;
    resp->return_code = 150;
    resp->return_value = 166;
    return resp;
}

void write_response(celery_ipc_response_t* response) {
    size_t size;
    char* packet = ipc_response_encode(response, &size);
    fwrite(packet, sizeof(char), size, stdout);
    fflush(stdout);
    free(packet);
}

int main(int argc, char const *argv[]) {
    // Handle sighangup when the port closes
    prctl(PR_SET_PDEATHSIG, SIGHUP);
    init_termios();

    celery_ipc_request_t* req;
    celery_ipc_response_t* resp;
    for(;;) {
        fprintf(stderr, "Waiting for new request \r\n");
        req = get_request();

        fprintf(stderr, "processing request. \r\n");
        resp = process_request(req);
        free(req->payload);
        free(req);

        fprintf(stderr, "writing response\r\n");
        write_response(resp);
        free(resp);
    }
    return 300;
}
