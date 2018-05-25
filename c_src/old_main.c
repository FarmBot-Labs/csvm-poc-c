// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
//
// // For sighangup handling.
// #include <signal.h>
// #include <sys/prctl.h>
// #include <linux/prctl.h>
//
// // Terminal settings
// #include <termios.h>
//
// // Thread
// #include <pthread.h>
//
// #include "celery_ipc.h"
// #include "util.h"
//
// /* Initialize new terminal i/o settings */
// void init_termios() {
//     struct termios settings;
//     tcgetattr(0, &settings); /* grab old terminal i/o settings */
//     settings.c_lflag &= ~ICANON; /* disable buffered i/o */
//     settings.c_lflag &= ~ECHO; /* set no echo mode */
//     tcsetattr(0, TCSANOW, &settings); /* use these new terminal i/o settings now */
// }
//
// celery_ipc_t ipc_buffer[256];
// celery_ipc_t* ipc_buffer_ptr;
// pthread_mutex_t lock;
//
// typedef enum ReaderState {
//   RIPC_REQUEST_HEADER = 0,
//   RIPC_REQUEST_PAYLOAD = 1,
//   RIPC_RESPONSE = 2
// } reader_state_t;
//
// reader_state_t reader = RIPC_REQUEST_HEADER;
//
// void* populate_buffer(void* arg) {
//       ipc_buffer_ptr = &ipc_buffer[0];
//
//       size_t number_of_bytes_needed = ipc_request_header_size();
//       size_t number_of_bytes_read = 0;
//       void* data = NULL;
//
//       char* buffer = malloc(sizeof(char) * number_of_bytes_needed);;
//       char* buffer_start = buffer;
//
//       FILE* reader_fp = stdin;
//       FILE* writer_fp = stdout;
//       fflush(reader_fp);
//       fflush(writer_fp);
//
//       for(;;) {
//         for(number_of_bytes_read = 0; number_of_bytes_read != number_of_bytes_needed;) {
//           buffer += fread(&buffer[0], sizeof(char), 1, reader_fp);
//           number_of_bytes_read += sizeof(buffer[0]);;
//         }
//
//         if(reader == RIPC_REQUEST_HEADER) {
//           celery_ipc_request_t* tmp_request = ipc_request_decode_header(buffer_start);
//           number_of_bytes_needed = tmp_request->payload_size;
//           data = tmp_request;
//           reader = RIPC_REQUEST_PAYLOAD;
//           buffer = buffer_start;
//           buffer = realloc(buffer, number_of_bytes_needed);
//           debug_print("Got IPC request header.\r\n");
//         } else if(reader == RIPC_REQUEST_PAYLOAD) {
//           celery_ipc_request_t* tmp_request = data;
//           number_of_bytes_needed = ipc_request_header_size();
//
//           celery_ipc_t ipc;
//           celery_ipc_value_t ipc_value;
//           ipc_value.request = tmp_request;
//
//           ipc.type = CELERY_IPC_REQUEST;
//           ipc.value = ipc_value;
//
//           reader = RIPC_REQUEST_HEADER;
//           buffer = buffer_start;
//           buffer = realloc(buffer, number_of_bytes_needed);
//           debug_print("Got IPC request payload.\r\n");
//
//           celery_ipc_response_t* resp = malloc(sizeof(celery_ipc_response_t));
//           resp->channel_number = ipc.value.request->channel_number;
//           resp->return_code = 150;
//           resp->return_value = 166;
//
//           size_t size;
//           char* packet = ipc_response_encode(resp, &size);
//           fwrite(packet, sizeof(char), size, writer_fp);
//           fflush(stdout);
//           free(packet);
//
//         } else if(reader == RIPC_RESPONSE) {
//
//         } else {
//           debug_print("something terrible happened: %d\r\n", reader);
//           exit(-1);
//         }
//
//       }
// }
//
// int main(int argc, char const *argv[]) {
//     // Handle sighangup when the port closes
//     prctl(PR_SET_PDEATHSIG, SIGHUP);
//     init_termios();
//
//     pthread_t thread;
//     pthread_mutex_init(&lock, NULL);
//     pthread_create(&thread, NULL, populate_buffer, NULL);
//     pthread_exit(NULL);
//     pthread_mutex_destroy(&lock);
//     return -1;
// }
