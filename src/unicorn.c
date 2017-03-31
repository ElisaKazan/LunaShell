/*
 * UNICORN.C
 * <insert description>
 */

#include "unicorn.h"

#include <stdlib.h>

// 2^16 will be our maximum command length.
#define COMMAND_LENGTH 65536

char command_buffer[COMMAND_LENGTH];

enum {
    RUNNING = 1,
    EXIT = 0
} status;

void error_ret(int ret, int quit_on_error) {
    if (ret == 0) {
        if (error == PERROR) {
            perror("unsh");
        }
        // If the error has an error message (for things like EOF we just quit silently)
        else if (errors[error]) {
            fprintf(stderr, "unsh: %s\n", errors[error]);
        }

        if (error == ENCOUNTERED_EOF || quit_on_error) {
            printf("\n");
            exit(1);
        }
    }
}


int main(int argc, char **argv) {
    status = RUNNING;

    int ret = prompt_init();
    error_ret(ret, 1);

    // Loop
    while(status == RUNNING) {
        // Display prompt
        ret = display_prompt();
        error_ret(ret, 0);

        // Read line
        ret = read_input(command_buffer, COMMAND_LENGTH, stdin);
        error_ret(ret, 0);

        // Evaluate & Print

        // Check status

        // Cleanup
        
    }
    return 0;
}

