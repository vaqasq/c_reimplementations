#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// use exec to launch shell
// set up repl
// scan and parse args
// fork and exec
// ignore signal handlers in parent prcoess but register INT/QUIT sigs in child process
// add special handling for cd, exit, history. There are others but just these for now
// Extra: Piping, >, >>, config files, & (sigchld required for async), etc


// Could countiously realloc to form a large array
// For proof of concept, I will just use a very large buffer
#define LINESIZE 1024

int main(void)
{
        // distable stdio buffering
        setvbuf(stdout, NULL, _IONBF, 0);

        char* arg_line = NULL;
        size_t arg_line_len = 0;
        char* token;

    while (1){

        /*
         * prompt
         */

        printf("ash > ");

        /*
         * get cmd input
         */

        if (-1 == getline(&arg_line, &arg_line_len, stdin)){
            perror("perror");
            exit(EXIT_FAILURE);
        }

        /*
         * make argc and argv
         */

        // most shell commands will be short. 5 is a solid starting length
        int bufLen = 5;
        int argc = 1;
        char **argv = malloc(bufLen * sizeof(char*));

        token = strtok(arg_line, " ");
        argv[0] = token;

        while (token != NULL){
            if (argc > bufLen){
                bufLen *= 2;
                if ((argv = realloc(argv, bufLen * sizeof(char*))) == NULL) {
                    perror("realloc");
                    exit(EXIT_FAILURE);
                }
            }
            argv[argc] = token;

            token = strtok(arg_line, " ");
            argc++;
        }

        free(argv);
    }

    free(arg_line);

}
