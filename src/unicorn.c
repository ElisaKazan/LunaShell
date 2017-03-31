/*
 * UNICORN.C
 * <insert description>
 */

#include "unicorn.h"

#include <stdlib.h>

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
        char *line;
        
        // Read line (function will dynamically allocate line)
        ret = read_input(stdin, &line);
        error_ret(ret, 0);

        // Evaluate & Print

        // Check status

        // Cleanup
        
    }
    return 0;
}

