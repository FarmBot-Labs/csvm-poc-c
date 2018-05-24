#include <stdio.h>
#include <stdlib.h>

// For sighangup handling.
#include <signal.h>
#include <sys/prctl.h>
#include <linux/prctl.h>

#include <termios.h>


#include "celery_ipc.h"
#include "util.h"

/** DELETEME */
#include <string.h>
#include <poll.h>
#include <errno.h>
#include <unistd.h>
#include "celery_script.h"
#include "celery_slicer.h"
#include "corpus.h"
#include "celery_heap_encoder.h"

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

    celery_ipc_request_t* request = ipc_request_decode_header(&request_header_buffer[0]);
    total_bytes_read = 0;

    while(total_bytes_read != request->payload_size) {
        request->payload[total_bytes_read] = getchar();
        total_bytes_read += 1;
    }

    return request;
}

celery_ipc_t ipc_buffer[256];
celery_ipc_t* ipc_buffer_ptr;
typedef enum ReaderState {
  RIPC_REQUEST_HEADER = 0,
  RIPC_REQUEST_PAYLOAD = 1,
  RIPC_RESPONSE = 2
} reader_state_t;

int main(int argc, char const *argv[]) {
    // Handle sighangup when the port closes
    prctl(PR_SET_PDEATHSIG, SIGHUP);
    init_termios();

    ipc_buffer_ptr = &ipc_buffer[0];

    reader_state_t reader = RIPC_REQUEST_HEADER;
    size_t number_of_bytes_needed = ipc_request_header_size();
    size_t number_of_bytes_read = 0;
    void* data = NULL;

    char* buffer = malloc(sizeof(char) * number_of_bytes_needed);
    char* buffer_start = buffer;

    fflush(stdin);

    for(;;) {
      for(number_of_bytes_read = 0; number_of_bytes_read != number_of_bytes_needed;) {
        buffer[0] = getchar();
        buffer += sizeof(buffer[0]);
        number_of_bytes_read += sizeof(buffer[0]);;
      }

      if(reader == RIPC_REQUEST_HEADER) {
        celery_ipc_request_t* tmp_request = ipc_request_decode_header(buffer_start);
        number_of_bytes_needed = tmp_request->payload_size;
        data = tmp_request;
        reader = RIPC_REQUEST_PAYLOAD;
        debug_print("Got IPC request header.\r\n");
      } else if(reader == RIPC_REQUEST_PAYLOAD) {
        celery_ipc_request_t* tmp_request = data;

        celery_ipc_t ipc;
        celery_ipc_value_t ipc_value;
        ipc_value.request = tmp_request;

        ipc.type = CELERY_IPC_REQUEST;
        ipc.value = ipc_value;

        memcpy(ipc_buffer_ptr, &ipc, sizeof(ipc));
        memcpy(ipc_buffer_ptr->value.request->payload, buffer_start, tmp_request->payload_size);

        reader = RIPC_REQUEST_HEADER;
        debug_print("Got IPC request payload.\r\n");
      } else if(reader == RIPC_RESPONSE) {

      } else {
        debug_print("something terrible happened: %d\r\n", reader);
        exit(-1);
      }

      debug_print("read %ld bytes.\r\n", number_of_bytes_read);
      memset(buffer_start, 0, sizeof(number_of_bytes_needed));
      buffer = buffer_start;
    }

    return -1;
}
