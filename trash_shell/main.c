#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// use exec to launch shell
// set up repl: Done
// scan and parse args, setting up argc, argv: Done
// fork and exec
// ignore signal handlers in parent prcoess but register INT/QUIT sigs in child process
// add special handling for cd, exit, history. There are others but just these for now
// Extra: Piping, >, >>, config files, & (sigchld required for async), etc


// Could argciously realloc to form a large array
// For proof of concept, I will just use a very large buffer
#define LINESIZE 1024

int main(void)
{
    // distable stdio buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    char* arg_line = NULL;
    size_t arg_line_len = 0;
    ssize_t nread;
    char* token;

    while (1){

        /*
         * prompt
         */

        printf("ash > ");

        /*
         * get cmd input
         */

        if ((nread = getline(&arg_line, &arg_line_len, stdin)) == -1){
            perror("perror");
            exit(EXIT_FAILURE);
        }

        // remove \n from end
        if(nread > 0 && arg_line[nread-1] == '\n') arg_line[nread-1] = '\0';

        /*
         * make argc and argv
         */

        int capacity = 8;
        int argc = 0;
        char **argv = malloc(capacity * sizeof(char *));

        char *token = strtok(arg_line, " \t\n");

        while (token) {
            if (argc >= capacity) {
                capacity *= 2;
                argv = realloc(argv, capacity * sizeof(char *));
            }
            argv[argc++] = token;
            token = strtok(NULL, " \t\n");
        }
        argv[argc] = NULL;  

        /*
         * Intercept if cd, history, exit
         */


        /*
         * fork and exec
         */


        free(argv);

    }

    free(arg_line);

}
