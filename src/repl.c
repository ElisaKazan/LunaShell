#include "unicorn.h"

#include <readline/readline.h>
#include <readline/history.h>

/*
 * REPL.C
 *
 * All the main REPL functionality of Unicorn Shell
 */

// 2^16 will be our maximum command length (if not reading from stdin)
#define COMMAND_LENGTH 65536

char command_buffer[COMMAND_LENGTH];

/* 
 * read_input 
 * returns 0 on error, 1 on success.
 */
int read_input(FILE *input, char **output) {
    int index = 0;
    if (input == stdin) {
        char *line = readline(get_prompt());

        // We encountered an EOF
        if (!line) {
            error = ENCOUNTERED_EOF;
            return 0;
        }

        if (strlen(line) != 0) {
            add_history(line);
        }

        *output = line;
    }
    else {
        int c = fgetc(input);

        while(c != EOF && c != '\n' && c != ';') {
            command_buffer[index] = c;
            index++;

            if (index >= COMMAND_LENGTH) {
                // ERROR: Command too long
                error = COMMAND_TOO_LONG;
                return 0;
            }

            c = fgetc(input);
        }

        if (c == EOF) {
            // ERROR: User entered EOF, quit.
            error = ENCOUNTERED_EOF;
            return 0;
        }

        // Done reading, add null character
        command_buffer[index] = '\0';
        *output = command_buffer;
    }

    // The first character is \0 (aka line is blank)
    if (!**output) {
        error = EMPTY_LINE;
        return 0;
    }

    return 1;
}

/* 
 * parse_input
 * 
 */
int parse_input(char *buffer, int length, command* command) {   
    char *ptr = strtok(buffer, " ");

    // Get command
    if(ptr != NULL) {
        command.path 
        command.path = ptr;
    }
    else {
        // ERROR: Insufficient arguments
    }

    // Get arguments
    while (ptr != NULL) {

    }

    return 0;
}

/*
 * execute
 * 
 */
int execute(command* command) {
    return 0;
}
