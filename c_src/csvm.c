#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "csvm.h"
#include "celery_ipc.h"

#include <termios.h>
#include <stdio.h>

static struct termios old, new;

/* Initialize new terminal i/o settings */
void initTermios(int echo)
{
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new = old; /* make new settings same as old settings */
    new.c_lflag &= ~ICANON; /* disable buffered i/o */
    if (echo) {
        new.c_lflag |= ECHO; /* set echo mode */
    } else {
        new.c_lflag &= ~ECHO; /* set no echo mode */
    }
    tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void)
{
    tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo)
{
    char ch;
    initTermios(echo);
    ch = getchar();
    resetTermios();
    return ch;
}

/* Read 1 character without echo */
char getch(void)
{
    return getch_(0);
}

/* Read 1 character with echo */
char getche(void)
{
    return getch_(1);
}

int request_header_size() {
    return sizeof(uint16_t) + NAMESPACE_SIZE + OPERATION_SIZE + sizeof(uint16_t) + (sizeof(char) * 2) + 1;
}

celery_ipc_request_t* get_request() {
    char request_header_buffer[request_header_size()];
    size_t total_bytes_read = 0;

    while(total_bytes_read != request_header_size()) {
        // fprintf(stderr, "reading header character\r\n");
        request_header_buffer[total_bytes_read] = getch();
        total_bytes_read += 1;
    }

    celery_ipc_request_t* request = decode_header(&request_header_buffer[0]);
    fprintf(stderr, "Got request packet. \r\n");
    fprintf(stderr, "\tchannel_number: %u \r\n", (unsigned int)request->channel_number);
    // fprintf(stderr, "\tnamespace: %s\r\n", request->namespace);
    // fprintf(stderr, "\toperation: %s\r\n", request->operation);
    // fprintf(stderr, "\tpayload_size: %u \r\n", (unsigned int)request->payload_size);
    fflush(stderr);
    total_bytes_read = 0;
    // while(total_bytes_read != request->payload_size) {
    //   request->payload[total_bytes_read] = getch();
    //   total_bytes_read += 1;
    // }



    return request;


    //
    // char request_header_buffer[request_header_size];
    // int tmp;
    // size_t did_read = read(stdin->_fileno, &request_header_buffer[0], request_header_size);
    // size_t total_bytes_read = 0;
    // if(did_read >= 0) {
    //     fprintf(stderr, "read data %ld bytes of data: %s\r\n", did_read, request_header_buffer);
    //     total_bytes_read += did_read;
    //     if(total_bytes_read >= request_header_size) {
    //       tmp = (unsigned)request_header_buffer[0] << 8 | (unsigned)request_header_buffer[1];
    //       request->channel_number = tmp
    //     }
    // } else {
    //   fprintf(stderr, "Failed to read data \r\n");
    // }
}

int main(int argc, char const *argv[]) {
    fprintf(stderr, "waiting for packet.\r\n");
    get_request();
    return 0;
}
