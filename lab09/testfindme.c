#include<stdio.h>
#include<unistd.h>
#include<getopt.h>
#include<string.h>
#include<stdlib.h>
#include "findme.h"

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: %s <starting directory> <file type> <maxdepth> <username> <file name>\n", argv[0]);
        return 1;
    }

    char *path = argv[1];
    int filetype = atoi(argv[2]);
    int maxdepth = atoi(argv[5]);
    char *username = argv[4];
    char *filename = argv[3];
	
    int res;
    /*res = readFromFile(path, filetype, filename, username, maxdepth);
    printf("FILE NAME FROM MAIN %s\n", filename);
    printf("RESULT FROM readFromFile %i\n", res);
    if(res == 0){
	printf("NOTHING!\n");
    }
    else{
	    printf("MATCHED! FILE FOUND!\n");

    }*/
    readFromFile(path, filetype, filename, username, maxdepth);
    return 0;
}
