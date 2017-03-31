// Methods and definitions for the Unicorn Shell (unsh)

#include <stdio.h>
#include <unistd.h>
#include <limits.h>

typedef struct {
    char **arguments; // The arguments to be given to the command

    char *output;
    char *input;
} command;

typedef struct {
    
} prompt;

// REPL functions
int read_input(FILE *file, char **output);
int parse_input(char *buffer, int len, command* command);
int execute(command* command);

// Prompt functions

int prompt_init();
char* get_prompt();

enum {
    PERROR,
    COMMAND_TOO_LONG,
    EMPTY_LINE,
    ENCOUNTERED_EOF
} error;

// Each item in here matches to an error code above in index
static char* errors[] = {
    NULL,
    "command too long",
    NULL,
    NULL
};
