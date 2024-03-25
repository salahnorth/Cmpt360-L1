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
    if (argc < 2) {
        printf("Usage: %s <starting directory> [<options>]\n", argv[0]);
        return 1;
    }
    

    struct stat statbuf;
    struct passwd *pwd;
    struct stat fileStat;

    char *path = ".";
    
    stat(path, &statbuf);
    
    pwd = getpwuid(statbuf.st_uid);
        
    char* username = pwd->pw_name;
    
    
    char* filetype = "z";
 
    int maxdepth = INT_MAX;
    char* filename = "";
    int fType;
    
    int flag = 0;
    
    for (int i = 1; i < argc; i += 1) {
    
        if (strcmp(argv[i], "find") == 0) {
        
            if (argc <= 2) break;
        
            else if (!((argv[i+1][0] == '.' && argv[i+1][1] == '/') || (argv[i+1][0] == '/') || (argv[i+1][0] == '-'))) {
                fprintf(stderr, "Invalid argument to find.\n");
                exit(EXIT_FAILURE);
}
            
            else if (argc > 2 && (argv[i+1][0] == '/' || (argv[i+1][0] == '.' && argv[i+1][1] == '/'))){  
                path = argv[i+1];
            }
                
        } else if (strcmp(argv[i], "-type") == 0) {
            if (i+1 >= argc) {
            
                fprintf(stderr, "Invalid argument to -type.\n");
                exit(EXIT_FAILURE);
            }
            
            else{
                filetype = argv[i+1];
            }
            
        } else if (strcmp(argv[i], "-name") == 0) {
            if (i+1 >= argc) {
            
                fprintf(stderr, "Invalid argument to -name.\n");
                exit(EXIT_FAILURE);
            }
            
            else{
            
                filename = argv[i+1];
                stat(path, &fileStat);
                fType = fileSystemTests(filename);
            
                if (filename != NULL) {
                    flag = 1;
                }
            }
            
        } else if (strcmp(argv[i], "-maxdepth") == 0) {
            if (i+1 >= argc) {
            
                fprintf(stderr, "Invalid argument to -maxdepth.\n");
                exit(EXIT_FAILURE);
            }
            
            else maxdepth = atoi(argv[i + 1]);
            
        } else if (strcmp(argv[i], "-user") == 0) {
            if (i+1 >= argc) {
            
                fprintf(stderr, "Invalid argument to -user.\n");
                exit(EXIT_FAILURE);
            }
            
            else username = argv[i + 1];
        }
    }
    
	//int flag = 0;
	/*char c1 = '/';
	char* pt;
	pt = &c1;*/
	
	
	//replace(path, pt); 
	//printf("Changing path %s\n", path);


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
    

    /*printf("PATH is %s\n", path);
    printf("FILETYPE is %d\n", fType);
    printf("FILENAME is %s\n", filename);
    printf("USERNAME is %s\n", username);
    printf("MAXDEPTH is %i\n", maxdepth);
    //printf("AFTER TESTING FILE TYPE %i\n", fType);*/
    readFromFile(path, fType, filename, username, maxdepth);
    return 0;
}

/*
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
}*/
