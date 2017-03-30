/*
 * UNICORN.C
 * <insert description>
 */

#include "unicorn.h"

// 2^16 will be our maximum command length.
#define COMMAND_LENGTH 65536

char command_buffer[COMMAND_LENGTH];

enum {
    RUNNING = 1,
    EXIT = 0
} status;


int main(int argc, char **argv) {
    status = RUNNING;
    prompt_init();

    // Loop
    while(status == RUNNING) {
        // Display prompt
        display_prompt();

        // Read line
        // Evaluate & Print

        // Check status

        // Cleanup
  
    }
    return 0;
}

