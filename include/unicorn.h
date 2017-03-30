// Methods and definitions for the Unicorn Shell (unsh)

#include <stdio.h>

typedef struct {
    char *path; // The (unresolved or resolved) name, will be checked 
                // against the PATH environmental variable
    char **arguments; // The arguments to be given to the command
} command;

typedef struct {
    
} prompt;

int read_input(char *buffer, int length, FILE *input);
int parse_input(char *buffer, int len, command* command);
int execute(command* command);
