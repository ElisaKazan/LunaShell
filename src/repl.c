#include "unicorn.h"

#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>

pid_t current_foreground_child = -1;

typedef struct {
    // Unqualified name (e.g. cd)
    const char *name;

    // Minimum and maximum including the command itself (e.g. 2 for cd). -1 if there is no min/max
    int min_args;
    int max_args;

    // handler function that takes a command and returns 1 or 0 depending on if it set error.
    int (*handler) (command*);
} builtin;

typedef struct alias {
    command *contents;
    struct alias *prev;
    struct alias *next;
} alias;

alias *head = NULL;
alias *tail = NULL;

int cd_handler(command *command);
int exit_handler(command *command);
int getenv_handler(command *command);
int setenv_handler(command *command);
int unset_handler(command *command);
int unicorn_handler(command *command);
int rainbowize_handler(command *command);

builtin builtins[] = {
    {
        .name = "cd",
        .min_args = 2,
        .max_args = 2,
        .handler = cd_handler
    },
    {
        .name = "exit",
        .min_args = 1,
        .max_args = 2,
        .handler = exit_handler
    },
    {
        .name = "getenv",
        .min_args = 2,
        .max_args = 2,
        .handler = getenv_handler
    },
    {
        .name = "setenv",
        .min_args = 3,
        .max_args = 3,
        .handler = setenv_handler
    },
    {
        .name = "unset",
        .min_args = 2,
        .max_args = 2,
        .handler = unset_handler
    },
    {
        .name = "unicorn",
        .min_args = 1,
        .max_args = 1,
        .handler = unicorn_handler
    },
    {
        .name = "rainbowize",
        .min_args = 1,
        .max_args = 1,
        .handler = rainbowize_handler
    }
};

const int NUM_BUILTINS = sizeof builtins / sizeof builtins[0];

/*
 * REPL.C
 *
 * All the main REPL functionality of Unicorn Shell
 */

// 2^16 will be our maximum command length (if not reading from stdin)
#define COMMAND_LENGTH 65536

char command_buffer[COMMAND_LENGTH];

int init_repl() {
    rl_catch_signals = 1;

    return 1;
}

/* 
 * read_input 
 * returns 0 on error, 1 on success.
 */
int read_input(FILE *input, char **output) {
    int index = 0;
    if (input == stdin) {
        // reset errno in case readline sets (we check if it's 0 below)
        char *prompt = get_prompt();

        if (!prompt) {
            return 0;
        }

        char *line = readline(prompt);

        // We encountered an EOF
        if (!line) {
            error = ENCOUNTERED_EOF;
            return 0;
        }

        if (strlen(line) != 0) {
            add_history(line);
        }

        *output = line;
    }
    else {
        int c = fgetc(input);

        while(c != EOF && c != '\n' && c != ';') {
            command_buffer[index] = c;
            index++;

            if (index >= COMMAND_LENGTH) {
                // ERROR: Command too long
                error = COMMAND_TOO_LONG;
                return 0;
            }

            c = fgetc(input);
        }

        if (c == EOF) {
            // ERROR: User entered EOF, quit.
            error = ENCOUNTERED_EOF;
            return 0;
        }

        // Done reading, add null character
        command_buffer[index] = '\0';
        *output = command_buffer;
    }

    if (!**output) {
        // ERROR: The first char is \0 (aka line is blank)
        error = EMPTY_LINE;
        return 0;
    }

    return 1;
}

/* 
 * parse_input
 * Loops over each character, adding \0 where the spaces are
 * to turn the char array into an array of char arrays (i.e. args)
 * Returns num arguments
 */
int parse_input(char *buffer, int length, command* command) {   
    int numArgs = 0;    // Argument counter
    int quote = 0;      // Quote state (0 = not quote, 1 = quotes)
    int isArg = 1;      // Arument state ( 0 = not arg, 1 = is arg)
    int i = 0;          // Loop counter
    
    int type;
    int len;

    char *currToken = buffer;

    // Loops over chars in buffer and adds '\0'
    for(i = 0; i < length; i++) {
        if (buffer[i] == '"') {
            // Handling quotes
            quote = !quote;
            buffer[i] = '\0';
            numArgs += isArg;
        }
        else if (buffer[i] == ' ' && quote == 0) {
            // Handling spaces outside of quotes
            buffer[i] = '\0';
            numArgs += isArg;
        }
        else if (buffer[i] == '\0') {
            numArgs += isArg;
        }
        else if (buffer[i] == '<' || buffer[i] == '>') {
            // No more arguments
            isArg = 0;
        }
        // Otherwise, just continue
    }

    // Check for invalid quotes
    if (quote == 1) {
        // ERROR: Invalid quotes
        error = INVALID_QUOTES;
        return 0;
    }

    // Allocate space for args
    command->arguments = malloc(sizeof(char*) * (numArgs + 1));
    command->num_args = numArgs;

    // Assign tokens to command variables
    i = 0;
    type = 0; // argument = 0, input = 1, output = 2
    len = 0;
    numArgs = 0;

    while (i < length) {

        len = strlen(currToken);

        if (strcmp(currToken, "<") == 0) {
            // Input
            if (command->input == NULL) {
                type = 1;
            }
            else {
                // ERROR: Duplicate input
                error = DUPLICATE_INPUT;
                return 0;
            }
        }
        else if (strcmp(currToken, ">") == 0) {
            // Output
            if (command->output == NULL) {
                type = 2;
            }
            else {
                // ERROR: Duplicate output
                error = DUPLICATE_OUTPUT;
                return 0;
            }
        }
        else if (type == 1) {
            // This token is an input 
            command->input = currToken;
        }
        else if (type == 2) {
            // This token is an output
            command->output = currToken;
        }
        else {
            // This token is an argument
            command->arguments[numArgs] = currToken;
            numArgs++;
        }

        // Increment token TODO shit might be broke here
        currToken += len;
        i += len;

        while (i < length && *currToken == '\0') {
            currToken++;
            i++;
        }
    }

    command->arguments[numArgs] = NULL;

    return 1;
}

builtin *search_builtins(command *command) {
    for (int i = 0; i < NUM_BUILTINS; i++) {
        if (strcmp(command->arguments[0], builtins[i].name) == 0) {
            return builtins + i;
        }
    }

    return NULL;
}

int execute_builtin(builtin *builtin, command *command) {
    if (builtin->min_args != -1 && command->num_args < builtin->min_args) {
        error = TOO_FEW_ARGS;
        return 0;
    }
    if (builtin->max_args != -1 && command->num_args > builtin->max_args) {
        error = TOO_MANY_ARGS;
        return 0;
    }

    return builtin->handler(command);
}

/*
 * execute
 * 
 */
int execute(command *command) {
    builtin *builtin = search_builtins(command);

    // Check for and execute builtins
    if (builtin) {
        return execute_builtin(builtin, command);
    }

    pid_t pid = fork();

    if (pid == 0) {
        // We're the child
        FILE *output_file = (command->output == NULL) ? stdout : fopen(command->output, "w");
        FILE *input_file = (command->input == NULL) ? stdin : fopen(command->input, "r");

        if (!output_file || !input_file) {
            perror("unsh");

            // Make sure we clean up, print the perror, then exit
            fclose(output_file);
            fclose(input_file);

            exit(1);
        }
        
        char *file = command->arguments[0];
        int ret = 0;
        ret = dup2(fileno(output_file), STDOUT_FILENO);
        ret = dup2(fileno(input_file), STDIN_FILENO);

        if (ret == -1) {
            perror("unsh");

            fclose(output_file);
            fclose(input_file);

            exit(1);
        }

        ret = dup2(fileno(input_file), STDIN_FILENO);

        if (ret == -1) {
            perror("unsh");

            fclose(output_file);
            fclose(input_file);

            exit(1);
        }
        
        ret = execvp(file, command->arguments);

        if (ret == -1) {
            perror("unsh");

            fclose(output_file);
            fclose(input_file);

            exit(1);
        }
        exit(0);
    }
    else if (pid == -1) {
        // We're the parent, but an error occurred and no child was created
        // Ask for the perror to be printed
        error = PERROR;
        return 0;
    }
    else {
        current_foreground_child = pid;
        
        // We're the parent, and the child was created and lives at pid `pid`
        int status;
        int ret;
        do {
            ret = waitpid(pid, &status, WNOHANG);
        } while (!WIFEXITED(status) && ret != -1);
        
        // ECHILD is an expected error - since we don't handle SIGCHLD and we do this inline
        // (to ensure that the processes don't clash) we just ignore it and move on, since we
        // can still get the info we need
        if (ret == -1 && errno != ECHILD) {
            current_foreground_child = -1;
            error = PERROR;
            return 0;
        }

        // WEXITSTATUS only returns the lower 8 bytes, meaning it can be at most 255
        // so it fits in a 4-char buffer, hence the magic number :)
        int exit_status = WEXITSTATUS(status);

        char exit_buffer[4];
        sprintf(exit_buffer, "%d", exit_status);

        setenv("?", exit_buffer, 1);

        //printf("Process %d exited with status code %d\n", pid, exit_status);

        current_foreground_child = -1;

        return 1;
    }
    
    return 0;
}


void free_stack_command(command *command) {
    free(command->arguments);
}

int cd_handler(command *command) {
    char *dir = expandpath(command->arguments[1]);
    printf("%s\n", dir);

    int ret = chdir(dir);

    // expandpath dynamically allocates this
    free(dir);

    if (ret == -1) {
        error = PERROR;
        return 0;
    }

    return 1;
}

int exit_handler(command *command) {
    int status = 0;
    char *watchptr;

    if (command->num_args == 2) {
        status = strtol(command->arguments[1], &watchptr, 10);

        // If the watchptr is not 0 (aka the whole string wasn't converted to a number)
        if (*watchptr) {
            error = INVALID_ARGUMENT;
            return 0;
        }
    }

    exit(status);
}

int getenv_handler(command *command) {
    char *target = command->arguments[1];

    char *val = getenv(target);

    if (!val) {
        fprintf(stderr, "No value for environmental variable %s\n", target);
    }
    else {
        printf("%s=%s\n", target, val);
    }

    return 1;
}

int setenv_handler(command *command) {
    char *key = command->arguments[1];
    char *value = command->arguments[2];

    if (setenv(key, value, 1) == -1) {
        error = PERROR;
        return 0;
    }

    return 1;
}

int unset_handler(command *command) {
    char *key = command->arguments[1];

    if (unsetenv(key) == -1) {
        error = PERROR;
        return 0;
    }

    return 1;
}

#define UNICORN_ASCII "   \\\r\n   _\\,,          \r\n  \"-=\\~     \r\n     \\\\~___~~\r\n    _|/---\\\\_\r\n    \\        \\  "

const char *unicorn_quotes[] = {
    "Push yourself because no one else is going to do it for you.",
    "Tomorrow may be too late.",
    "Take a chance.",
    "Follow your heart.",
    "Because the people who are crazy enough to think they can change the world are the ones who do.",
    "In the end, we only regret the chances we didn't take.",
    "Coffee? Coffee.",
    "Don't regret anything you do, because in the end it makes you who you are.",
    "There are seven days in the week and someday isn't one of them.",
    "Go for a walk to clear your head.",
    "The surest way to find your dream job is to create it.",
    "*highfive*",
    "Be happy! It drives people crazy.",
    "Experiment. Fail. Learn. Repeat."
};

int unicorn_handler(command *command) {
    printf("%s\n\n", UNICORN_ASCII);

    srand(time(NULL));

    printf("Unicorn says: %s\n", unicorn_quotes[rand()%sizeof(unicorn_quotes)/sizeof(unicorn_quotes[0])]);

    return 1;
}

int rainbowize_handler(command *command) {
    rainbowize = !rainbowize;

    return 1;
}
