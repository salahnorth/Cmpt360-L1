#include<stdio.h>
#include<unistd.h>
#include<getopt.h>
#include<string.h>
#include<stdlib.h>
#include "findme.h"

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: %s <starting directory> <file type> <maxdepth> <username> <file name>\n");
        return 1;
    }

    const char *path = argv[1];
    int filetype = atoi(argv[2]);
    int maxdepth = atoi(argv[3]);
    char *username = argv[4];
    char *filename = argv[5];

    readFromFile(path, filetype, maxdepth, username, filename);

    return 0;
}
