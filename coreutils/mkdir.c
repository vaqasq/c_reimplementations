#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>

// Just use mkdir(), parent folders must exist for simplicity. No flags

int main(int argc, char* argv[]){

    if (argc != 2){
        fprintf(stderr, "Error: Incorrect usage of mkdir. See man page.");
        return EXIT_FAILURE;
    }

    if (mkdir(argv[1], 0777) == -1){
        fprintf(stderr, "Error creating directory: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
