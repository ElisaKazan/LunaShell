// Methods and definitions for the Unicorn Shell (unsh)

#include <stdio.h>
#include <unistd.h>
#include <limits.h>

typedef struct {
    char *path; // The (unresolved or resolved) name, will be checked 
                // against the PATH environmental variable
    char **arguments; // The arguments to be given to the command
} command;

typedef struct {
    
} prompt;

// REPL functions
int read_input(char *buffer, int length, FILE *input);
int parse_input(char *buffer, int len, command* command);
int execute(command* command);

// Prompt functions

int prompt_init();
int display_prompt();

enum {
    PERROR,
    COMMAND_TOO_LONG,
    ENCOUNTERED_EOF
} error;

// Each item in here matches to an error code above in index
static char* errors[] = {
    NULL,
    "command too long",
    NULL
};
