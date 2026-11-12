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
 * problem: even during waitpid, sigchld is still sent. Use sigprocmask() to block SIGCHLD if async_cmd == true;
 * Piping, >, >>, config files, & (sigchld required for async), etc
 * Extra: add history(special case), other coreutils
 */

// SIGINT signal handler for parent process
static bool interrupt_signal_recieved = false;

static void parent_sigint_handler(int sig){
    interrupt_signal_recieved = true;
}

void sigchld_handler(int sig) {
    int status;
    pid_t pid;

    // Loop drains all pending terminated children
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) { // Child exited normally
            printf("[%d] process finished\n", pid);
        } else if (WIFSIGNALED(status)) { // Child terminated by a signal (e.g. SIGKILL, SIGSEGV)
            printf("[%d] process killed\n", pid);
        } else {
            printf("[%d] process finished abnormally\n", pid);
        }
    }

}

int main(void)
{

    // register SIGINT handler.
    // Not as portable as sigaction()
    signal(SIGINT, parent_sigint_handler);

    // register SIGCHLD handler.
    struct sigaction sa = {
        .sa_handler = sigchld_handler,
        .sa_flags   = SA_RESTART | SA_NOCLDSTOP, // SA_NOCLDSTOP: Only fire on termination
    };
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);

    // disable stdio buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    char* arg_line = NULL;
    size_t arg_line_len = 0;
    ssize_t nread;
    char* token;
    bool async_cmd;

    while (1){

        async_cmd = false;
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
            continue;
        }

        // remove \n from end
        if(nread > 0 && arg_line[nread-1] == '\n') arg_line[nread-1] = '\0';

        // check for & async
        if (nread > 0 && arg_line[nread-2] == '&') async_cmd = true;

        /*
         * make argc and argv
         */

        int capacity = 8;
        int argc = 0;
        char **argv = malloc(capacity * sizeof(char *));

        token = strtok(arg_line, " \t\n");

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


        } else if (0 == (strcmp("exit", argv[0]))){
            if (argc != 2){
                fprintf(stderr, "-trash: kill usage: kill <pid>\n");
                free(argv);
                continue;
            }

            pid_t p = (pid_t)atoi(argv[1]);
            if (p == 0){
                fprintf(stderr, "-trash: invalid pid\n");
                free(argv);
                continue;
            }

            if (-1 == kill(p, SIGTERM)){
                fprintf(stderr, "-trash: kill failed\n");
                free(argv);
                continue;
            }
            printf("[%d] killed\n",p);
            free(argv);
            continue;
        }

        /*
         * fork and exec
         */

        const char* home = getenv("HOME");
        char cmd_buf[100];

        int written = snprintf(cmd_buf, sizeof(cmd_buf), "%s/.personal_utils/%s", home, argv[0]);

        if (written == -1){
            fprintf(stderr, "-trash: error during snprintf\n");
            free(argv);
            continue;
        } else if ((size_t)written >= sizeof(cmd_buf)){
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
            default: // parent process
                    if (!async_cmd) waitpid(pid,NULL,0);
                    else {
                        printf("[%d] process running in the background\n", pid);
                        async_cmd = false;
                    }
        }
        
        free(argv);

    }

    return EXIT_SUCCESS;

}


