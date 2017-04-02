#include "unicorn.h"
#include <time.h>
#include <string.h>

char *username;
char hostname_buffer[HOST_NAME_MAX];

#define PROMPT_MAX 256

char prompt_buffer[PROMPT_MAX];

// Function Prototypes
int adjust_prompt(char *buffer, char *string, int p);
char *get_date(char *date_buf, int length);
char *get_time(char *time_buf, int length);
char *get_directory(char *dir_buf, int length);
int min(int a, int b);

/*
 * prompt_init
 * <description> TODO
 */
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

/*
 * get_prompt
 * <description> TODO
 */
char *get_prompt() {
    int length = strlen("[\\u@\\h \\w]\\U $");
    char user_data[length + 1];
    strcpy(user_data, "[\\u@\\h \\w]\\U $");

    char working_buffer[PROMPT_MAX];

    int i;      // user_data index
    int p = 0;  // prompt_buffer index

    int is_special = 0; // Special boolean (0 = not special, 1 = special) 

    for (i = 0; i < length && p < PROMPT_MAX - 1; i++) {
        if (is_special == 1) {
            // TODO switch
            switch(user_data[i]) {
                case 'd':
                    // Date
                    p = adjust_prompt(prompt_buffer, get_date(working_buffer, PROMPT_MAX), p);
                    break;
                case 'h':
                    // Hostname
                    p = adjust_prompt(prompt_buffer, hostname_buffer, p);
                    break;
                case 's':
                    // Shell 
                    p = adjust_prompt(prompt_buffer, SHELL_NAME, p);
                    break;
                case 'u':
                    // Username
                    p = adjust_prompt(prompt_buffer, username, p);
                    break;
                case 'U':
                    // Unicorn Emoji
                    p = adjust_prompt(prompt_buffer, UNICORN, p);
                    break;
                case 't':
                    // Time
                    p = adjust_prompt(prompt_buffer, get_time(working_buffer, PROMPT_MAX), p); 
                    break;
                case 'w':
                    // Current Working Directory
                    p = adjust_prompt(prompt_buffer, get_directory(working_buffer, PROMPT_MAX), p);
                    break;
                case '\\':
                    // Backslash
                    prompt_buffer[p] = '\\';
                    p += 1;
                    break;
                default:
                    // Invalid escape character
                    error = INVALID_ESC;
                    return 0;
            }

            is_special = 0;
        }
        else if (user_data[i] == '\\') {
            // Speacial character
            is_special = 1;
        }
        else {
            // Regular character
            prompt_buffer[p] = user_data[i];
            p += 1;
        }
    }

    prompt_buffer[p] = '\0';

    snprintf(prompt_buffer, PROMPT_MAX, "[%s@%s %s]🦄 $ ", username, hostname_buffer, getcwd(NULL, 0));
    return prompt_buffer;
}

/*
 * adjust_prompt
 * Takes the buffer, string to put into buffer and index p in buffer
 * Returns the new p.
 */

int adjust_prompt(char *buffer, char *string, int p) {
    int n = PROMPT_MAX - 1 - p;
    strncpy(buffer + p, string, n);
    p += min(n, strlen(string));

    return p;
}

// HELPER FUNCTIONS

char *get_date(char *date_buf, int length) {
    time_t rawtime = time(NULL);    // rawtime used to create time instance
    struct tm *timeinfo;            // time instance
             
    timeinfo = localtime(&rawtime); 
    strftime(date_buf, length, "%a %b %d", timeinfo);

    return date_buf;
}

char *get_time(char *time_buf, int length) {
    time_t rawtime = time(NULL);    // rawtime used to create time instance
    struct tm *timeinfo;            // time instance
    
    timeinfo = localtime(&rawtime);
    strftime(time_buf, length, "%H:%M:%S", timeinfo);

    return time_buf;
}

char *get_directory(char *dir_buf, int length) {
    dir_buf = getcwd(NULL, 0);
    return dir_buf;
}

int min(int a, int b) {
    if (a < b) {
        return a;
    }

    return b;
}
