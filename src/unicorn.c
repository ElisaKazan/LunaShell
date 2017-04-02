/*
 * UNICORN.C
 * <insert description>
 */

#include "unicorn.h"

#include <stdlib.h>
#include <string.h>

#include <pwd.h>

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
    "invalid escape character",
    "too few arguments to builtin command",
    "too many arguments to builtin command",
    "invalid argument to builtin command"
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

        if (error == INVALID_ESC || quit_on_error) {
            printf("\n");
            exit(1);
        }
    }
}

void repl(FILE *input, int fataleof) {
    int ret;
    char *line;
    int line_len;
    
    // Loop
    while(status == RUNNING) {
        // Read line (function will dynamically allocate line)
        ret = read_input(input, &line);
        error_ret(ret, 0);

        if (!ret && error == ENCOUNTERED_EOF) {
            if (fataleof) {
                printf("\n");
                exit(0);
            }
            else {
                return;
            }
        }

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
        if (input == stdin) {
            free(line);
        }

        free_stack_command(&command);
    }
}

char *expandpath(const char *path) {
    if (path[0] == '~' && (path[1] == '/' || path[1] == '\0')) {
        int pathlen = strlen(path);
        const char *homedir = getpwuid(getuid())->pw_dir;
        int homelen = strlen(homedir);

        char *new_path = malloc(pathlen + homelen + 3);

        strcpy(new_path, homedir);
        strcpy(new_path + homelen, path + 1);
        return new_path;
    }

    return realpath(path, NULL);
}

int main(int argc, char **argv) {
    status = RUNNING;

    int ret = prompt_init();
    error_ret(ret, 1);

    ret = register_signals();
    error_ret(ret, 1);

    ret = init_repl();
    error_ret(ret, 1);

    char *unshrcpath = expandpath("~/.unshrc");
    //printf("Looking for unshrc at %s", unshrcpath);
    FILE *unshrc = fopen(unshrcpath, "r");

    if (unshrc) {
        // Don't die on EOFs, they're normal in files :D
        repl(unshrc, 0);
    }

    fclose(unshrc);
    free(unshrcpath);

    // Die on EOF
    repl(stdin, 1);

    return 0;
}

