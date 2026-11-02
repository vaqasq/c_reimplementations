// Linux and Macos, not Windows compliant

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

int main(void){

    // Quite sure this should never fail given this use case
    struct passwd *user = getpwuid(getuid());
    if (user == NULL){
        fprintf(stderr, "user not found");
        return EXIT_FAILURE;
    }
    puts(user->pw_name);
    return EXIT_SUCCESS;

}
