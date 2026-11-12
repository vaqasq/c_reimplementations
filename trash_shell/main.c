#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/*
* use exec to launch shell
* Consider adding custom malloc and arena allocator
* ignore signal handlers in parent prcoess but register INT/QUIT sigs in child process for &
* Piping, >, >>, config files, & (sigchld required for async), etc
* Extra: add history(special case), other coreutils
*/

// SIGINT signal handler for parent process
bool interrupt_signal_recieved = false;

static void parent_sigint_handler(int sig){
    interrupt_signal_recieved = true;
}

// write a child_siginit_handler. Consider _Exit() over exit() for async safety, perhaps it wont matter.

int main(void)
{


    // register SIGINT handler.
    // Not as portable as sigaction()
    signal(SIGINT, parent_sigint_handler);

    // disable stdio buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    char* arg_line = NULL;
    size_t arg_line_len = 0;
    ssize_t nread;
    char* token;

    while (1){

        /*
         * prompt
         */

        printf("trash > ");

        /*
         * get cmd input
         */

        if ((nread = getline(&arg_line, &arg_line_len, stdin)) == -1){
            perror("getline");
            exit(EXIT_FAILURE);
        }

        // check if ^C was recived.
        if (interrupt_signal_recieved){
            interrupt_signal_recieved = false;
            fprintf(stdout, "SIGINT recieved, input ignored\n");
            continue;
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
         * Intercept if exit and cd.
         * Consider adding a history utility
         */

       if (0 == (strcmp("exit", argv[0]))){ 
            if (argc != 1){
                fprintf(stderr, "-trash: exit takes no extra inputs\n");
                free(argv);
                continue;
            }
            free(argv);
            exit(0);
        } else if (0 == (strcmp("cd", argv[0]))){ // Will only effect the working process
            if (argc == 1){
                fprintf(stderr, "-trash: please provide a directory\n");
                free(argv);
                continue;
            }
            else if (argc > 2){
                fprintf(stderr, "-trash: please provide a singular directory\n");
                free(argv);
                continue;
            }
            
            if (-1 == chdir(argv[1])){
                fprintf(stderr, "cd failed\n");
                free(argv);
                continue;

            }

            printf("You are now in dir: %s\n", argv[1]);
            free(argv);
            continue;


        }

        /*
         * fork and exec
         */

        const char* home = getenv("HOME");
        char cmd_buf[100];

        int written = snprintf(cmd_buf, sizeof(cmd_buf), "%s/.personal_utils/%s", home, argv[0]);

        if (written >= sizeof(cmd_buf)){
            fprintf(stderr, "Sorry, your cmd's name is too large\n");
            free(argv);
            continue;
        }
    
        pid_t pid = fork();
        
        switch(pid){
            case 0: ; // execute
                if (-1 == execv(cmd_buf, argv)){
                    fprintf(stderr, "exec failed: likely an invalid command. see man page\n");
                    exit(1);
                }
            case -1:
                fprintf(stderr, "fork failed\n");
                free(argv);
                continue;
            default: // parent process, wait for child to be done
               waitpid(pid,NULL,0);
        }

        free(argv);

    }

    return EXIT_SUCCESS;

}


