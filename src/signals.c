#include "unicorn.h"

#include <signal.h>
#include <string.h>

#include <readline/readline.h>

void interrupt_handler(int signal) {
    if (current_foreground_child != -1) {
        // Forward the signal on to the child
        kill(current_foreground_child, signal);
    }
    else {
        // Reprompt
        printf("\n");
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

int register_signals() {
    struct sigaction action;
    memset(&action, 0, sizeof(action));

    action.sa_handler = interrupt_handler;
    
    int ret = sigaction(SIGINT, &action, NULL);

    if (ret == -1) {
        error = PERROR;
        return 0;
    }

    return 1;
}
