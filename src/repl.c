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
int read_input(char *buffer, int length, int input) {
    

    // Check length of file
    if(1 <= length) {
    }
    else {
        // Command is too long
        // ERROR 
    }


    return 0;
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
