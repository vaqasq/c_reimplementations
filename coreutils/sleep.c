#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/* Goals
 * Accept number that defaults to second
 * Accept number that ends with either s,m,h, or d for unit
 * Both previous + be able to process infinite && for additional commands
 */

int main(int argc, char* argv[]){

    if (argc != 2){
        char* err = "Improper use of sleep. See man page.";
        fprintf(stderr, "%s\n", err);
        return EXIT_FAILURE;
    }

    char* end;
    double sleepLength; 

    errno = 0;
    sleepLength = strtod(argv[1], &end);

    if (errno == EINVAL || errno == ERANGE) {
        perror("strtol");
        return EXIT_FAILURE;
    }

    if (0 == strcmp("", end) || 0 == strcmp("s", end)){
        sleep((unsigned int)sleepLength);
    }
    else if (0 == strcmp("m", end)){
        sleep((unsigned int)(sleepLength * 60));
    }
    else if (0 == strcmp("h", end)){
        sleep((unsigned int)(sleepLength * 3600));
    }
    else if (0 == strcmp("d", end)){
        sleep((unsigned int)(sleepLength * 86400));
    }
    else {
        fprintf(stderr, "Improper use of sleep. See man page.");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
