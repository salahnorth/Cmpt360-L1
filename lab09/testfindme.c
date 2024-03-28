#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<limits.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<pwd.h>
#include<pthread.h>
#include"findme.h"


/**
  *@file testfindme.c
  *@brief This file runs the main program to find a specific file
  *@author Salah Mohamed & Vy Tran
  *@date 27032024
  *@version 8
  */
int main(int argc, char *argv[]) {

    // Check if the minimum arguments (2) is satisfied
    if (argc < 2) {
        printf("Usage: %s <starting directory> [<options>]\n", argv[0]);
        return 1;
    }
    

    struct stat statbuf;
    struct passwd *pwd;
    struct stat fileStat;

    // Default variables
    char *path = NULL;
    
    stat(path, &statbuf);
    pwd = getpwuid(statbuf.st_uid);
        
    char* username = pwd->pw_name;
    char* filetype = "z";
    int maxdepth = INT_MAX;
    char* filename = "";
    int fType;
    
    int fileFlag = 0;
    int findFlag = 0;
    
    for (int i = 1; i < argc; i += 1){
        if(argc <= 2){
     		path = ".";   
        	break;
	}
	
	// Set default path = '.' if no path is provided
        if(strcmp(argv[i], "find") == 0){
	    if(argv[2][0] == '-'){
	        path = ".";
	        findFlag = 1;
	 
	    }

            // Set path if path is provided
	    else if((argv[2][0] == '/' || argv[2][0] == '.')){  
                path = argv[2];
            }
       
            // Invalid find option
            else if(((argv[2][0] != '.' && argv[2][0] != '/') || (argv[3][0] != '-')) && findFlag == 0){
                fprintf(stderr, "Invalid argument to find.\n");
                exit(EXIT_FAILURE);
	    }	
        } 
        
        // Invalid type option, else set filetype
	else if(strcmp(argv[i], "-type") == 0){
            if (i+1 >= argc) {
                fprintf(stderr, "Invalid argument to -type.\n");
                exit(EXIT_FAILURE);
            }
            else{
                filetype = argv[i+1];
            }
        }
        
        // Invalid name option, else set filename
	else if(strcmp(argv[i], "-name") == 0){
            if(i+1 >= argc){
                fprintf(stderr, "Invalid argument to -name.\n");
                exit(EXIT_FAILURE);
            }
            else{
                filename = argv[i+1];
                stat(path, &fileStat);
                fType = get_fileType(path, filename);
            
                if (filename != NULL) {
                    fileFlag = 1;
                }
            }
            
        } 
        
        // Invalid maxdepth option, else set maxdepth
	else if(strcmp(argv[i], "-maxdepth") == 0){
            if(i+1 >= argc){
                fprintf(stderr, "Invalid argument to -maxdepth.\n");
                exit(EXIT_FAILURE);
            }
            else maxdepth = atoi(argv[i + 1]);
            
        } 
        
        // Invalid user option, else set username
	else if(strcmp(argv[i], "-user") == 0){
            if(i+1 >= argc){
                fprintf(stderr, "Invalid argument to -user.\n");
                exit(EXIT_FAILURE);
            }
            
            else username = argv[i + 1];
        }
    }
    
        char c1 = '/';
        char* pt;
        pt = &c1;
    
        replaceStr(path, pt); // Replace character in path string


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
	
	else if(*filetype == 'z' && fileFlag == 0){
	    fType = 5;
	   
	}
	
	
    findFile(path, fType, filename, username, maxdepth);
    return 0;
}
