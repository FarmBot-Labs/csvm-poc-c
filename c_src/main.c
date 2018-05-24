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
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdio.h>


int
main(void)
{
    int status, result, i;
    double sum;
    lua_State *L;

    /*
     * All Lua contexts are held in this structure. We work with it almost
     * all the time.
     */
    L = luaL_newstate();

    luaL_openlibs(L); /* Load Lua libraries */

    /* Load the file containing the script we are going to run */
    status = luaL_loadfile(L, "script.lua");
    if (status) {
        /* If something went wrong, error message is at the top of */
        /* the stack */
        fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
        exit(1);
    }

    /*
     * Ok, now here we go: We pass data to the lua script on the stack.
     * That is, we first have to prepare Lua's virtual stack the way we
     * want the script to receive it, then ask Lua to run it.
     */
    lua_newtable(L);    /* We will pass a table */

    /*
     * To put values into the table, we first push the index, then the
     * value, and then call lua_rawset() with the index of the table in the
     * stack. Let's see why it's -3: In Lua, the value -1 always refers to
     * the top of the stack. When you create the table with lua_newtable(),
     * the table gets pushed into the top of the stack. When you push the
     * index and then the cell value, the stack looks like:
     *
     * <- [stack bottom] -- table, index, value [top]
     *
     * So the -1 will refer to the cell value, thus -3 is used to refer to
     * the table itself. Note that lua_rawset() pops the two last elements
     * of the stack, so that after it has been called, the table is at the
     * top of the stack.
     */
    for (i = 1; i <= 5; i++) {
        lua_pushnumber(L, i);   /* Push the table index */
        lua_pushnumber(L, i*2); /* Push the cell value */
        lua_rawset(L, -3);      /* Stores the pair in the table */
    }

    /* By what name is the script going to reference our table? */
    lua_setglobal(L, "foo");

    /* Ask Lua to run our little script */
    result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (result) {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        exit(1);
    }

    /* Get the returned value at the top of the stack (index -1) */
    sum = lua_tonumber(L, -1);

    printf("Script returned: %.0f\n", sum);

    lua_pop(L, 1);  /* Take the returned value out of the stack */
    lua_close(L);   /* Cya, Lua */

    return 0;
}
