/*
 * UNICORN.C
 * <insert description>
 */

#include "unicorn.h"

#include <stdlib.h>
#include <string.h>

enum {
    RUNNING = 1,
    EXIT = 0
} status;

// Each item in here matches to an error code in the error enum
static char* errors[] = {
    "unspecified error",
    NULL,
    "command too long",
    NULL,
    NULL,
    "duplicate inputs",
    "duplicate outputs",
    NULL,
    "unmatched quote(s)",
    "invalid escape character"
};

void error_ret(int ret, int quit_on_error) {
    if (ret == 0) {
        if (error == PERROR) {
            perror("unsh");
        }
        // If the error has an error message (for things like EOF we just quit silently)
        else if (errors[error]) {
            fprintf(stderr, "unsh: %s\n", errors[error]);
        }

        if (error == INVALID_ESC || error == ENCOUNTERED_EOF || quit_on_error) {
            printf("\n");
            exit(1);
        }
    }
}

int main(int argc, char **argv) {
    status = RUNNING;

    char *line;
    int line_len;

    int ret = prompt_init();
    error_ret(ret, 1);

    ret = register_signals();
    error_ret(ret, 1);

    ret = init_repl();
    error_ret(ret, 1);

    // Loop
    while(status == RUNNING) {
        // Read line (function will dynamically allocate line)
        ret = read_input(stdin, &line);
        error_ret(ret, 0);

        if (!ret) {
            continue;
        }

        line_len = strlen(line) + 1;

        // Parse
        command command;
        memset(&command, 0, sizeof(command));

        ret = parse_input(line, line_len, &command);
        error_ret(ret, 0);

        if (!ret) {
            continue;
        }

        // Evaluate/Execute
        ret = execute(&command);
        error_ret(ret, 0);

        if (!ret) {
            continue;
        }

        // Check status

        // Cleanup
        free(line);

        free_stack_command(&command);
    }
    return 0;
}

