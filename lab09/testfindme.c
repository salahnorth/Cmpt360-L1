#include<stdio.h>
#include<unistd.h>
#include<getopt.h>
#include<string.h>
#include<stdlib.h>
#include "findme.h"
#include <limits.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<pwd.h>



void replace(char*, char*);

int main(int argc, char *argv[]) {
    /*(if (argc != 6) {
        printf("Usage: %s <starting directory> <file type> <file name> <user name> <maxdepth>\n", argv[0]);
        return 1;
    }*/
    
    
    
    
    struct stat statbuf;
    struct passwd *pwd;
        
    
    
    

    char *path = "./";
    
    lstat(path, &statbuf);
    
    pwd = getpwuid(statbuf.st_uid);
        
    char* username = pwd->pw_name;
    
    int exfType = fileSystemTests("findme.c");
    printf("FILE TYPE EXAMPLE IS %i\n", exfType);
    
    char* filetype = "z";
 
    int maxdepth = INT_MAX;
    //char *username = NULL;
    char *filename = NULL;
    int fType;
    printf("\n\n\n\n\n\n\nFILE TYPE IN TEST %s\n", filetype);
    
    int flag = 0;
    
    for (int i =0; i < argc; i++){
    
        if (strcmp(argv[i], "find") == 0){
            path = argv[i+1];
        }
        
        if (strcmp(argv[i], "-type") == 0){
            filetype = argv[i+1];
        }
        
        if (strcmp(argv[i], "-name") == 0){
            filename = argv[i+1];
            
            fType = fileSystemTests(filename);
            printf("AFTER TESTING FILE TYPE %i\n", fType);
            
            if(filename != NULL){
                flag = 1;
                printf("GOING TO FILENAME IF!!!!\n");
            }
            
        }
        
        if (strcmp(argv[i], "-maxdepth") == 0){
            maxdepth = atoi(argv[i+1]);
        }
        
        if (strcmp(argv[i], "-user") == 0){
            username = argv[i+1];
        }
        
    }
        
        
        
	
	//./lab9 -type f
	
	
	
	
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
	
	else if(*filetype == 'z' && flag == 0){
	    fType = 5;
	   
	}

    printf("PATH is %s\n", path);
    printf("FILETYPE is %s\n", filetype);
    printf("FILENAME is %s\n", filename);
    printf("USERNAME is %s\n", username);
    printf("MAXDEPTH is %i\n", maxdepth);
    //printf("AFTER TESTING FILE TYPE %i\n", fType);
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
