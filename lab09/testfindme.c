#include<stdio.h>
#include<unistd.h>
#include<getopt.h>
#include<string.h>
#include<stdlib.h>
#include "findme.h"


void replace(char*, char*);

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: %s <starting directory> <file type> <file name> <user name> <maxdepth>\n", argv[0]);
        return 1;
    }

    char *path = argv[1];
    char* filetype = argv[2];
 
    int maxdepth = atoi(argv[5]);
    char *username = argv[4];
    char *filename = argv[3];
    int fType;
	printf("\n\n\n\n\n\n\nFILE TYPE IN TEST %s\n", filetype);
	
	//int flag = 0;
	char c1 = '/';
	char* pt;
	pt = &c1;
	
	
	replace(path, pt); 
	printf("Changing path %s\n", path);


	if(*filetype == 'f'){
		fType = 0;
	}
	else if(*filetype == 'd'){
		fType = 1;
	}
	else if(*filetype == 'c'){
		fType = 2;
	}
	else if(*filetype == 'b'){
		fType = 3;
	}
	else if(*filetype == 's'){
		fType = 4;
	}

    readFromFile(path, fType, filename, username, maxdepth);
    return 0;
}


void replace(char* s, char* c){
	int counter = 0;
	for(int i=0; s[i] != '\0'; i++){
		printf("GOING IN FOR\n");
		if(s[i] != *c){
			continue;
		}
		if(s[strlen(s) - 1] == *c){
			s[strlen(s) -1] = '\0';
		}
	}
}
