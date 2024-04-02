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
#include <glob.h>

/**
  *@brief This function converts char* filetype to integer values
  *@author Salah Mohamed & Vy Tran
  *@date 
  *@version 
  */

int filetypeCalc(char* filetype){

    int fType;
    
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
    
    return fType;
}

/**
  *@file testglob.c
  *@brief This file tests the main program for find a specific file with specific patterns
  *@author Salah Mohamed & Vy Tran
  *@date 
  *@version 
  */
int main(int argc, char *argv[]) {

    struct stat statbuf;
    struct passwd *pwd;

    // Default variables
    char *path = ".";
    
    stat(path, &statbuf);
    pwd = getpwuid(statbuf.st_uid);
        
    char* username = pwd->pw_name;
    
    char* filetype = "f";
    
    int intFiletype = filetypeCalc(filetype);
    
    int maxdepth = 5;
    
    char* filename1 = "\\.c$"; // For pattern= *.c
    char* filename2 = ".ap\\.c"; // For pattern= ?ap.c
    char* filename3 = "[c]*p\\.c$"; // For pattern= [c]*p.c
    char* filename4 = "^[^c]*\\.c$"; // For pattern= [^c]*.c
    
    char* filename5 = "code.*\\.c"; // For pattern= code*.c
    char* filename6 = "co.e\\.c"; // For pattern co?e.c
    
	
    printf("Testing ./lab10 find %s -type %s -name %s -user %s -maxdepth %d...\n\n", path, filetype, filename1, username, maxdepth);
    findFile(path, intFiletype, filename1, username, maxdepth);
    
    printf("Testing ./lab10 find %s -type %s -name %s -user %s -maxdepth %d...\n\n", path, filetype, filename2, username, maxdepth);
    findFile(path, intFiletype, filename1, username, maxdepth);
    
    printf("\nTesting ./lab10 find %s -type %s -name %s -user %s -maxdepth %d...\n\n", path, filetype, filename3, username, maxdepth);

    findFile(path, intFiletype, filename3, username, maxdepth);
    
    printf("\nTesting ./lab10 find %s -type %s -name %s -user %s -maxdepth %d...\n\n", path, filetype, filename4, username, maxdepth);

    findFile(path, intFiletype, filename4, username, maxdepth);
    
    printf("\nTesting ./lab10 find %s -type %s -name %s -user %s -maxdepth %d...\n\n", path, filetype, filename5, username, maxdepth);

    findFile(path, intFiletype, filename5, username, maxdepth);
    
    printf("\nTesting ./lab10 find %s -type %s -name %s -user %s -maxdepth %d...\n\n", path, filetype, filename6, username, maxdepth);
    findFile(path, intFiletype, filename6, username, maxdepth);
    
    return 0;
}
