#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "include/model.h"
#include "include/controller.h"

void handle_input(const char* input, ShmBuf* shmp, char** response) {
    if (strncmp(input, "@msg ", 5) == 0) {
        const char* msg = input + 5;
        model_send_message(shmp, msg);
        *response = model_read_messages(shmp);
    } else {
        *response = model_execute_command(input);
    }
}
