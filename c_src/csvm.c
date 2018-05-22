#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <linux/prctl.h>

#include "csvm.h"
#include "celery_ipc.h"

#include <termios.h>
#include <stdio.h>


/* Initialize new terminal i/o settings */
void initTermios()
{
    struct termios settings;
    tcgetattr(0, &settings); /* grab old terminal i/o settings */
    settings.c_lflag &= ~ICANON; /* disable buffered i/o */
    settings.c_lflag &= ~ECHO; /* set no echo mode */
    tcsetattr(0, TCSANOW, &settings); /* use these new terminal i/o settings now */
}

celery_ipc_request_t* get_request() {
    char request_header_buffer[request_header_size()];
    size_t total_bytes_read = 0;

    while(total_bytes_read != request_header_size()) {
        char read = getchar();
        fprintf(stderr, "0x%04X ", read);
        request_header_buffer[total_bytes_read] = read;
        total_bytes_read += 1;
    }

    fprintf(stderr, "\r\n");
    celery_ipc_request_t* request = decode_header(&request_header_buffer[0]);
    fprintf(stderr, "Got request packet. \r\n");
    fprintf(stderr, "\tchannel_number: %u \r\n", (unsigned int)request->channel_number);
    fprintf(stderr, "\tnamespace: %s\r\n", request->namespace);
    fprintf(stderr, "\toperation: %s\r\n", request->operation);
    fprintf(stderr, "\tpayload_size: %u \r\n", (unsigned int)request->payload_size);
    fflush(stderr);
    total_bytes_read = 0;

    while(total_bytes_read != request->payload_size) {
        request->payload[total_bytes_read] = getchar();
        total_bytes_read += 1;
        fprintf(stderr, "\r num bytes: %ld out of %u: %s", total_bytes_read, (unsigned int)request->payload_size, request->payload);
    }

    fprintf(stderr, "\r\nok.\r\n");

    return request;
}

int main(int argc, char const *argv[]) {
    prctl(PR_SET_PDEATHSIG, SIGHUP);
    initTermios();
    fprintf(stderr, "waiting for packet.\r\n");
    celery_ipc_request_t* req = get_request();
    fprintf(stderr, "got request\r\n");
    size_t size;
    char* packet = ipc_request_encode(req, &size);
    fwrite(packet, sizeof(char), size, stdout);
    return 300;
}
