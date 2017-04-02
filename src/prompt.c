#include "unicorn.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

char *username;
char hostname_buffer[HOST_NAME_MAX];
char *ps1;
int rainbowize = 1; // Rainbow boolean (1 = rainbow, 0 = not rainbow)
int c = 1;

#define PROMPT_MAX 256*5 // for rainbow

char prompt_buffer[PROMPT_MAX];

// Function Prototypes
int adjust_prompt(char *buffer, char *string, int p);
int insert_char(char *buffer, char character, int p);
int insert_string(char *buffer, char *string, int p);
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

    // Get or Set PS1 
    ps1 = getenv("PS1");

    if (!ps1) {
        //ps1 = "ABCDEFG- \\u HIJKLMNOP";
        ps1 = "[\\u@\\h \\w]\\U$ ";
        setenv("PS1", ps1, 1);
    }

    return 1;
}

/*
 * get_prompt
 * <description> TODO
 */
char *get_prompt() {
    int length = strlen(ps1);
    char user_data[length + 1];
    strcpy(user_data, ps1);

    char working_buffer[PROMPT_MAX];

    int i;      // user_data index
    int p = 0;  // prompt_buffer index

    int is_special = 0; // Special boolean (0 = not special, 1 = special) 

    for (i = 0; i < length && p < PROMPT_MAX - 9; i++) {
        if (is_special == 1) {
            // TODO switch
            switch(user_data[i]) {
                case 'd':
                    // Date
                    p = insert_string(prompt_buffer, get_date(working_buffer, PROMPT_MAX), p);
                    //p = adjust_prompt(prompt_buffer, get_date(working_buffer, PROMPT_MAX), p);
                    break;
                case 'h':
                    // Hostname
                    p = insert_string(prompt_buffer, hostname_buffer, p);
                    //p = adjust_prompt(prompt_buffer, hostname_buffer, p);
                    break;
                case 's':
                    // Shell 
                    p = insert_string(prompt_buffer, SHELL_NAME, p);
                    //p = adjust_prompt(prompt_buffer, SHELL_NAME, p);
                    break;
                case 'u':
                    // Username
                    p = insert_string(prompt_buffer, username, p);
                    //p = adjust_prompt(prompt_buffer, username, p);
                    break;
                case 'U':
                    // Unicorn Emoji
                    p = insert_string(prompt_buffer, UNICORN, p);
                    //p = adjust_prompt(prompt_buffer, UNICORN, p);
                    break;
                case 't':
                    // Time
                    p = insert_string(prompt_buffer, get_time(working_buffer, PROMPT_MAX), p);
                    //p = adjust_prompt(prompt_buffer, get_time(working_buffer, PROMPT_MAX), p); 
                    break;
                case 'w':
                    // Current Working Directory
                    p = insert_string(prompt_buffer, get_directory(working_buffer, PROMPT_MAX), p);
                    //p = adjust_prompt(prompt_buffer, get_directory(working_buffer, PROMPT_MAX), p);
                    break;
                case '\\':
                    // Backslash
                    p = insert_char(prompt_buffer, '\\', p);
                    //prompt_buffer[p] = '\\';
                    //p += 1;
                    break;
                default:
                    // Invalid escape character
                    error = INVALID_ESC;
                    return NULL;
            }

            is_special = 0;
        }
        else if (user_data[i] == '\\') {
            // Speacial character
            is_special = 1;
        }
        else {
            // Regular character
            p = insert_char(prompt_buffer, user_data[i], p);
            //prompt_buffer[p] = user_data[i];
            //p += 1;
        }
    }

    prompt_buffer[p] = '\0';
    strcat(prompt_buffer, "\001\e[0m\002");

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

int insert_char(char *buffer, char character, int p) {
    if (rainbowize) {
        int len = 7;
        int n = PROMPT_MAX - 1 - p;
        
        if (len + 1 > n) {
            return p;
        }

        snprintf(buffer + p, n, "\001\e[3%dm\002", c);
        c = (c + 1) % 7;
        p += min(n, len);
    }
    
    buffer[p] = character;
    buffer[p + 1] = '\0';
    p += 1;

    return p;
}

int insert_string(char *buffer, char *string, int p) {
    int n = PROMPT_MAX - 1 - p;
    int len = strlen(string);
    int i;

    if (rainbowize) {
        for (i = 0; i < len; i++) {
            char curr = string[i];
            p = insert_char(buffer, curr, p);
        }
    }
    else {
        strncpy(buffer + p, string, n);
        p += min(n, strlen(string));
    }

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
    getcwd(dir_buf, length);

    return dir_buf;
}

int min(int a, int b) {
    if (a < b) {
        return a;
    }

    return b;
}
