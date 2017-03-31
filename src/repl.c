#include "unicorn.h"

/*
 * REPL.C
 *
 * All the main REPL functionality of Unicorn Shell
 */

/* 
 * read_input 
 * returns 0 on error, 1 on success.
 */
int read_input(char *buffer, int length, FILE *input) {
    
    int index = 0;
    int c = fgetc(input);

    while(c != EOF && c != '\n' && c != ';') {
        buffer[index] = c;
        index++;

        if (index >= length) {
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
    buffer[index] = '\0';

    return 1;
}

/* 
 * parse_input
 * 
 */
int parse_input(char *buffer, int len, command* command) {
    return 0;
}

/*
 * execute
 * 
 */
int execute(command* command) {
    return 0;
}
