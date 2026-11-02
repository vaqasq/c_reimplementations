// Implement the following flags: -l for stats, -a for if it starts with .

#include <sys/resource.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

void printDir(DIR *dir, char *dirName, bool allFlag, bool statFlag);

int main(int argc, char *argv[]){

    // Hardcode just ./ls
    if (argc == 1){
        DIR* currDir = opendir(".");

        printDir(currDir, "Current Dir", false, false);
        return EXIT_SUCCESS;
    }

    // Flags, a, l
    bool listAllFlag = false;
    bool longInfoFlag = false;
    char* flag_str = "al";

    int opt;
    while ((opt = getopt(argc, argv, flag_str)) != -1){
        switch (opt){
            case 'a':
                listAllFlag = true;
                break;
            case 'l':
                longInfoFlag = true;
                break;
            case '?':
                fprintf(stderr, "Improper flag. See man page.\n");
                return EXIT_FAILURE;
        }
    }

    // global var optind, an index into argv, is now the directories;

    // Assume size 1 for now, otherwise realloc
    DIR **directoriesArr = malloc(sizeof(DIR*));
    char **directoriesNameArr = malloc(sizeof(char*));
    // The number of directory entries will be the same for both their DIR* and their names
    int numDirectoryEntries;

    // If no specific folders were provided, assume it is just the current folder
    if (optind >= argc) {

        numDirectoryEntries = 1;
        directoriesArr[0] = opendir(".");
        directoriesNameArr[0] = malloc((strlen("Current Dir") + 1) * sizeof(char));
        strcpy(directoriesNameArr[0], "Current Dir");

    }
    // Else, there are other options that need to be acted upon;
    else {

        directoriesArr = realloc(directoriesArr, (argc-1) * sizeof(DIR *));
        numDirectoryEntries = 0;

        DIR *currDirFlag;
        for (int i = optind; i < argc; ++i){

            currDirFlag = opendir(argv[i]);

            if (NULL == currDirFlag){
                fprintf(stderr, "Error opening file '%s': %s\n", argv[i], strerror(errno));
                return EXIT_FAILURE;
            }

            directoriesArr[numDirectoryEntries] = currDirFlag;

            // Add name aswell

            directoriesNameArr[numDirectoryEntries] = malloc((strlen(argv[i]) + 1) * sizeof(char));
            strcpy(directoriesNameArr[numDirectoryEntries], argv[i]);

            numDirectoryEntries++;

        }

    }

    // work with dirs, -l, -a
    for (int i = 0; i < numDirectoryEntries; ++i){
        printDir(directoriesArr[i], directoriesNameArr[i], listAllFlag, longInfoFlag);
    }


    // closing open directories
    for (int i = 0; i < numDirectoryEntries; ++i){
        closedir(directoriesArr[i]);
    }

    return EXIT_SUCCESS;
}

void printDir(DIR *dir, char *dirName, bool allFlag, bool statFlag){

    printf("%s:\n", dirName);

    struct dirent *entry;
    struct stat sb;
    int fd = dirfd(dir);

    while ((entry = readdir(dir)) != NULL){
        if (entry->d_name[0] != '.' || allFlag){
            if (statFlag) {
                if (fstatat(fd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW) == -1) {
                    perror("lstat");
                    fprintf(stderr, "%s\n", entry->d_name);
                    exit(EXIT_FAILURE);
                }
                // unix time stamp lol
                printf("%ld\t%lld\t%s\n", sb.st_atime, sb.st_size, entry->d_name);

            } else {
                printf("%s   ", entry->d_name);
            }
        }
    }
    printf("\n");
}

