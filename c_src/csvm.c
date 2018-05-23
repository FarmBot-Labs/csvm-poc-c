#include <stdio.h>
#include <stdlib.h>

// For sighangup handling.
#include <signal.h>
#include <sys/prctl.h>
#include <linux/prctl.h>

#include <termios.h>

#include "csvm.h"
#include "celery_ipc.h"
#include "util.h"

/** DELETEME */
#include <string.h>
#include "celery_script.h"
#include "celery_slicer.h"
#include "corpus.h"

csvm_t* GlobalVM = NULL;

csvm_t* csvm_init() {
    csvm_t* csvm = malloc(sizeof(csvm_t));
    for(int i = 0; i<NUMBER_REGISTERS; i++) {
        csvm->registers[i] = 0;
    }
    csvm->registers[REGISTER_PC] = -1;
    celery_heap_t* heap = heap_init();
    csvm->heap = heap;
    return csvm;
}

void csvm_tick(csvm_t* vm) {
    debug_print("Tick start\r\n");
    debug_print("Increment PC\r\n");
    vm->registers[REGISTER_PC]+=1;
    csvm_register_t pc = vm->registers[REGISTER_PC];
    debug_print("Fetch heap entry at pc: %d\r\n", pc);
    if(!(pc >= vm->heap->heap_size)) {
        debug_print_q("\r\n\r\n");
        celery_heap_entry_t* entry = vm->heap->entries[pc];
        debug_print_q("Executing: [%s](%ld) {", string_from_node_name(entry->kind), entry->number_entries);
        for(int i = 0; i<entry->number_entries; i++) {
          celery_heap_entry_kv_t* kv = entry->kvs[i];
          char key_str[20];
          char val_str[20];
          strcpy(&key_str[0], "X");
          switch(kv->type) {
            case CSH_BOOL:
              strcpy(&key_str[0], string_from_arg_name(kv->key));
              strcpy(&val_str[0], kv->value.bool_value ? "true" : "false");
              break;
            case CSH_NUMBER:
              strcpy(&key_str[0], string_from_arg_name(kv->key));
              sprintf(&val_str[0], "%f", kv->value.number_value);
              break;
            case CSH_ADDRESS:
              switch(kv->key) {
                case HEAP_BODY:
                  strcpy(&key_str[0], "HEAP_BODY");
                  break;
                case HEAP_NEXT:
                  strcpy(&key_str[0], "HEAP_NEXT");
                  break;
                case HEAP_LINK:
                  strcpy(&key_str[0], "HEAP_LINK");
                  break;
                case HEAP_PARENT:
                  strcpy(&key_str[0], "HEAP_PARENT");
                  break;
                case HEAP_KIND:
                  strcpy(&key_str[0], "HEAP_KIND");
                  break;
                default:
                  strcpy(&key_str[0], string_from_arg_name(kv->key));
                  break;
              }
              sprintf(&val_str[0], "0x%d", (int)kv->value.number_value);
              break;
            case CSH_STRING:
              strcpy(&key_str[0], string_from_arg_name(kv->key));
              sprintf(&val_str[0], "%s", kv->value.str_value);
              break;
            default:
              strcpy(&val_str[0], "unknown type");
          }
          debug_print_q("\r\n\t'%s': '%s', ", key_str, val_str);
        }
        debug_print_q("\r\n}\r\n\r\n");
    }
}

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
    celery_ipc_response_t* resp = malloc(sizeof(celery_ipc_response_t));
    resp->channel_number = request->channel_number;
    resp->return_code = 150;
    resp->return_value = 166;

    if(strcmp(request->namespace, "TICK") == 0) {
        csvm_tick(GlobalVM);
    }

    if(strcmp(request->namespace, "LOAD") == 0) {
        celery_script_t* celery = buffer_to_celery_script(request->payload);
        GlobalVM->heap = slice(celery, GlobalVM->heap);
    }
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
    GlobalVM = csvm_init();

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

    return 300;
}
