// Methods and definitions for the Unicorn Shell (unsh)

#define _DEFAULT_SOURCE
#define SHELL_NAME "unsh"
#define UNICORN "🦄 "

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

typedef struct {
    char **arguments; // The arguments to be given to the command

    int num_args;
    
    char *output;
    char *input;
} command;

// Main file funcs
char *expandpath(const char *path); // Returns a malloc'd buffer

// REPL functions
int init_repl();
int read_input(FILE *file, char **output);
int parse_input(char *buffer, int len, command* command);
int execute(command* command);
void free_stack_command(command *command);

extern pid_t current_foreground_child;

// Prompt functions

int prompt_init();
char* get_prompt();

extern int rainbowize;

// Signal handling functions
int register_signals();

enum {
    UNSPECIFIED,
    PERROR,
    COMMAND_TOO_LONG,
    EMPTY_LINE,
    ENCOUNTERED_EOF,
    DUPLICATE_INPUT,
    DUPLICATE_OUTPUT,
    FILE_NOT_FOUND,
    INVALID_QUOTES,
    INVALID_ESC,
    TOO_FEW_ARGS,
    TOO_MANY_ARGS,
    INVALID_ARGUMENT
} error;
