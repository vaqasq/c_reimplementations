#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int printLogicalPath();
int printPhysicalPath();

int main(int argc, char *argv[]){

    if (argc > 2){
        fprintf(stderr, "Too many arguments");
        return EXIT_FAILURE;
    }
    else if (argc == 1 || strcmp("-L", argv[1]) == 0){
        int rc;
        if ((rc = printLogicalPath()) == 1){
            return EXIT_FAILURE;
        }
    }
    else if (strcmp("-P", argv[1]) == 0){
        int rc;
        if ((rc = printPhysicalPath()) == 1){
            return EXIT_FAILURE;
        }
    }
    else {
        fprintf(stderr, "Improper use of pwd");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


// Default or -L
int printLogicalPath(){

    char* logicalPath = getenv("PWD");

    if (logicalPath != NULL)
        printf("%s\n", logicalPath);
    else{
        perror("");
        return 1;
    }

    return 0;
}

// -P, resolve symbolic links
int printPhysicalPath(){

    long size;
    char *buf;
    char *pwd_str;

    size = pathconf(".", _PC_PATH_MAX);

    if ((buf = (char *)malloc((size_t)size)) != NULL)
        pwd_str = getcwd(buf, (size_t)size);
    else{
        perror("");
        return 1;
    }
        
    printf("%s\n", pwd_str);
    return 0;
}
