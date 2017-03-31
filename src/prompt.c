#include "unicorn.h"

char *username;
char hostname_buffer[HOST_NAME_MAX];

#define PROMPT_MAX 256

char prompt_buffer[PROMPT_MAX];

int prompt_init() {
    // This is better than storing in a buffer because it will be
    // changed when the euid changes
    username = getlogin();
    if (username == NULL) {
        return 0;
    }

    if (gethostname(hostname_buffer, HOST_NAME_MAX) == -1) {
        return 0;
    }

    return 1;
}

char *get_prompt() {
    snprintf(prompt_buffer, PROMPT_MAX, "[%s@%s %s]🦄 $ ", username, hostname_buffer, getcwd(NULL, 0));
    return prompt_buffer;
}
