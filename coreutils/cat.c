#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define COUNT 100

int meow(int inputFD);

int main(int argc, char *argv[]){

    // If just ./cat or ./cat -
    if (argc == 1 || (argc == 2 && (strcmp("-", argv[1]) == 0))){
        int rc = meow(STDIN_FILENO);
        return rc;
    }

    // Otherwise, cat - filenames, or cat filenames

    int *fdArr = malloc((argc-1) * sizeof(*fdArr));
    int entries_fdArr = 0;
    bool dashPresent = false;


    for (int i = 1; i < argc; ++i){

        if (strcmp(argv[i], "-") == 0){
            if (dashPresent)
                continue;
            fdArr[entries_fdArr] = STDIN_FILENO;
            entries_fdArr++;
            dashPresent = true; 
            continue;
        }

        // just open files now/check/add to arr
        int fd = open(argv[i], O_RDONLY);

        if (fd == -1){
            fprintf(stderr, "Failed to open file '%s': %s\n", argv[i], strerror(errno));
            return 1;
        }

        fdArr[entries_fdArr] = fd;
        entries_fdArr++;
        
    }


    // cat files

    int rc;

    for (int i = 0; i < entries_fdArr; ++i){

       rc = meow(fdArr[i]);
       if (rc == 1){
           fprintf(stderr, "Error in catting a fileA");
           return 1;
       }

    }

    free(fdArr);
    
    // close opened files other than stdin
    
    for (int i = 0; i < entries_fdArr; ++i){
        if (fdArr[i] != STDIN_FILENO){
            close(fdArr[i]);
        }
    }

    return 0;
}

// meow cuz cat
int meow(int inputFD){

    char     buf[COUNT] = {0};
    ssize_t  readin;
    ssize_t  written;

    while ((readin = read(inputFD, buf, COUNT))){

        if (readin < 0){
            switch errno{
                case EAGAIN:
                case EINTR:
                    continue;
            }
            perror("");
            return 1;
        }

        written = write(STDOUT_FILENO, buf, readin);

        if (written < 0){
            switch errno{
                case EAGAIN:
                case EINTR:
                    continue;
            }
            perror("");
            return 1;
        }
    }
    return 0;

}


