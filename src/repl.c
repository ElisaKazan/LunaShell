#include "unicorn.h"

#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>

/*
 * REPL.C
 *
 * All the main REPL functionality of Unicorn Shell
 */

// 2^16 will be our maximum command length (if not reading from stdin)
#define COMMAND_LENGTH 65536

char command_buffer[COMMAND_LENGTH];

/* 
 * read_input 
 * returns 0 on error, 1 on success.
 */
int read_input(FILE *input, char **output) {
    int index = 0;
    if (input == stdin) {
        char *line = readline(get_prompt());

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

        while (*currToken == '\0') {
            currToken++;
            i++;
        }
    }

    // TESTING
    for (i = 0; i < numArgs; i++) {
        printf("Argument %d: %s\n", i, command->arguments[i]);
    }
    printf("Output: %s\n", command->output);
    printf("Input: %s\n", command->input);
    return 1;
}

/*
 * execute
 * 
 */
int execute(command* command) {
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

        if (ret == -1) {
            perror("unsh");

            fclose(output_file);
            fclose(input_file);

            exit(1);
        }

        execvp(file, command->arguments);
        
        exit(0);
    }
    else if (pid == -1) {
        // We're the parent, but an error occurred and no child was created
        // Ask for the perror to be printed
        error = PERROR;
        return 0;
    }
    else {
        // We're the parent, and the child was created and lives at pid `pid`
        int status;
        do {
            waitpid(pid, &status, 0);
        } while (WIFEXITED(status));

        int exit_status = WEXITSTATUS(status);

        printf("Process %d exited with status code %d\n", pid, exit_status);
        
        return 1;
    }
    
    return 0;
}
