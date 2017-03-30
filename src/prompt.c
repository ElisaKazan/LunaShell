#include "unicorn.h"

char username_buffer[LOGIN_NAME_MAX];
char hostname_buffer[HOST_NAME_MAX];

int prompt_init() {
    getlogin_r(username_buffer, LOGIN_NAME_MAX);
    gethostname(hostname_buffer, HOST_NAME_MAX);
}

int display_prompt() {
    printf("[%s@%s %s] $ ", username_buffer, hostname_buffer, getwd(NULL));
}
