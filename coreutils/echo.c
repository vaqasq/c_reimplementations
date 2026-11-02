#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Hard coding input should be fine, echo is a very simple command.
int main(int argc, char *argv[]){

    // If the command is simple echo, no -n or anything else
    if (argc == 1){
        printf("\n");
        return 0;
    }

    // If the command is echo -n. -n means to not print the trailing -n, so no
    // printing or puts is needed
    if (argc == 2 && (strcmp(argv[1],"-n") == 0)){
        return 0;
    }

    // Other

    bool addNewline = true;
    bool nFlagAcceptable = true;

    for (int i = 1; i < argc; ++i){
        if ((strcmp(argv[i], "-n") == 0) && nFlagAcceptable){
            addNewline = false;
        }
        else {
            printf("%s ", argv[i]);
            nFlagAcceptable = false;
        }
    }

    if (addNewline){
        printf("\n");
    }

    return 0;

}
