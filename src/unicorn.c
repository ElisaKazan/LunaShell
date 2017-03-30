/*
 * UNICORN.C
 * <insert description>
 */

enum {
    RUNNING = 1,
    EXIT = 0
} status;


int main(int argc, char **argv) {
    //
    status = RUNNING;

    // Loop
    while(status == RUNNING) {
        // Display prompt
        display_prompt();
        
        // Read line

        // Evaluate & Print

        // Check status
        status = RUNNING; // TODO

        // Cleanup
  
    }
    return 0;
}

