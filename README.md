# LunaShell

## Background

A shell is a program that executes programs on behalf of the user.

* A shell is probably the most popular REPL (Read Evaluate Print Loop)
 * Read: Read in the command from the shell
 * Evaluate: Splits the input into one command and many arguments (`ls`, `-la`, `The Directory`). This step handles quoting, escaping, etc.
 * Print: Takes the parsed input from the Evaluate stage, and calls the relevent system calls in the `fork` and `exec` family. This step also handles output and input redirection using `freopen`
 * Loop: Do the previous three things until there is an exit signal or the "exit" command is used

* A shell is probably the most popular REPL (Read Evaluate Print Loop). Let’s work through an example:
 * Read: Read in the command string from the shell (read `ls -la “The Directory”` from the prompt)
 * Evaluate: Splits the input into one command and many arguments (`ls`, `-la`, `The Directory`). This step handles quoting, escaping, etc.
 * Print: Takes the parsed input from the Evaluate stage, and calls the relevent system calls in the `fork` and `exec` family. This step also handles output and input redirection using `freopen` (run the `ls` command with the given args, let it execute, wait for it to finish)
 * Loop: Do the previous three things until there is an exit signal or the "exit" command is used

We referenced this: https://brennan.io/2015/01/16/write-a-shell-in-c/

## Goals and Expected Result

Our goal from this project is to learn more about how shells work internally and the technology and techniques that goes into them. We also wish to gain a deep understanding of the shell's role in the operating system and the user's interaction with it.

We expect to develop a shell with the following capabilities:
* Command interpretation (see background)
* Customizable shell prompt (you can change the PS1 variable to change the prompt)
* Finding & executing a command on the PATH (starting with a default system path, but allowing customization by users)
* Executing an executable via an absolute or relative path
* Provide intelligable and useful error messages (if the executable doesn't have execute permissions, say that, don't just throw up on the user)
* Managing child processes (always wait until the process is finished before re-prompting, forward signals via control-c, control-d, control-z, and handling signals being sent to the process)
* Builtin commands (`cd`, `exit`)
* Basic input/output redirection capabilities (commands like `ls -lah > test` will write the output of the `ls` command to the file `test`)
* A couple initialization options (`-x` to print commands as they are executed)
* Initialization files (~/.lushrc)
* Customize prompt, aliases
* Quoting of arguments that contain spaces

## Evaluation Plan

Requirements:
* Written in the C programming language for POSIX-compatible OS’s
* Can execute commands on the PATH
* Can run executables based on relative pathname (ex: ./myExecutable param)
* Can execute builtin functions
* Has an initialization file which is executed on creation of a shell process (for customizing prompt, etc)
* Has a PS1 environment variable to modify the prompt based on a format string
* 

## Timeline



