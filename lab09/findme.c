#include<dirent.h>
#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>
#include<stdio.h>
#include<pwd.h>
#include<sys/wait.h>
#include"findme.h"

/**
  *@brief Searching for a specific file
  *@param path, filetype, filename, username, maxdepth
  *@return 0 or 1
  */
int findFile(char *path, int filetype,char* filename,char* username, int maxdepth){

    int rootFlag = 0;
    
    struct dirent *entry;
    DIR *dp;
    
    struct stat statbuf;
    
    dp = opendir(path);
    
    if(dp == NULL){
        perror("opendir");
        return 0;
    }
    
    if(stat(path, &statbuf) == -1){
         perror("stat failed");
         exit(EXIT_FAILURE);
         return 0;
     }
            	    
    while((entry = readdir(dp))){
        if(stat(path, &statbuf) == -1)
            continue;
    
        FILE *fp1;

	if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
	    continue;
	}
	
	char *filePath = (char*) malloc((strlen(path)+1) + (strlen(entry->d_name)+1));
	
	filePath = strcpy(filePath, path);
	
	if (strcmp(filePath, "/") == 0){
	     rootFlag = 1;
	}
	
	if (rootFlag == 0){
	    strcat(filePath, "/");
	    strcat(filePath, entry->d_name);
	}
	else if(rootFlag != 0){
	    strcat(filePath, entry->d_name);
	}
	
	int filetypeFromTest = fileSystemTests(filePath);
	
	if(maxdepth > 0){
	    if(filetypeFromTest == 1){
	        pid_t pid = fork();
	        if (pid == -1){
	            perror("fork");
	            exit(EXIT_FAILURE);
	            
	        }
	        else if (pid == 0){
	            findFile(filePath,filetype,filename, username, maxdepth - 1);
	            closedir(dp);
	            exit(EXIT_SUCCESS);
	        }
	    
	    }
	    if(filetype == filetypeFromTest || filetype == DEFAULT_FILETYPE){
	    
	        fp1 = fopen(filePath, "r");

	        if(fp1 == NULL){
	            continue;
	        }
	        else{
	            struct stat fileStat;
                    if (stat(filePath, &fileStat) == 0){
                        struct passwd *pwd = getpwuid(fileStat.st_uid);
                        
                        if (pwd != NULL && strcmp(username, pwd->pw_name) == 0) {
                            if (strcmp(entry->d_name, filename) == 0 || strcmp(filename, "") == 0){       	
                                printPath(filePath);
			        free(filePath);
                            }
                        }
                        
                        else{
			    printf("Can't access, wrong username\n");
			    return 1;
	                }
                    }
                    
                    else{
	                printf("Can't access, stat file error\n");
	                return 1;
	                
	            }

                    fclose(fp1);
	        }
	        
	    }

        }
        
    }
	
    closedir(dp);
    while (wait(NULL) > 0); // Wait for child processes to finish
    return 0;
 
}


/**
  *@brief Handling filename and path to check the input file's type (loop)
  *@param path, filename
  *@return -1, 0, 1, 2, 3, 4, 5, 6, 7 
  */
int get_fileType(char *path, char* filename){
    
    struct dirent *entry;
    DIR *dp;
    
    struct stat statbuf;
    
    dp = opendir(path);
    
    if(dp == NULL){
        perror("opendir");
        return 0;
    }
    
    if(stat(path, &statbuf) == -1){
         perror("stat failed");
         exit(EXIT_FAILURE);
         return 0;
     }
    
    while((entry = readdir(dp))){
        if(stat(path, &statbuf) == -1)
            continue;
    
        FILE *fp1;

	if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
	    continue;
	}
	
	char *filePath = (char*) malloc((strlen(path)+1) + (strlen(entry->d_name)+1));
	
	filePath = strcpy(filePath, path);
	strcat(filePath, "/");
	strcat(filePath, entry->d_name);
	
	int filetypeFromTest = fileSystemTests(filePath);
	
	
	    if(filetypeFromTest == 1 && strcmp(entry->d_name, filename) != 0){
	
                get_fileType(filePath, filename);
	    }
	
	    else{
	    
	        fp1 = fopen(filePath, "r");

	        if(fp1 == NULL){
	            continue;
	        }
	        else{
	            struct stat fileStat;
                    if (stat(filePath, &fileStat) == 0){
                            if (strcmp(entry->d_name, filename) == 0){
			        free(filePath);
			        return filetypeFromTest;
                            }   
                    }
                    fclose(fp1);   	
	        }   
	    }
    }
	
    closedir(dp);
    return 0;
 
}


/**
  *@brief Check file type
  *@param filename
  *@return -1, 0, 1, 2, 3, 4, 5, 6, 7
  */
int fileSystemTests(char *filename){
     struct stat buff;

     int tmp;
     tmp = lstat(filename, &buff);

     if(tmp!=0){
             return(-1);	// Error
     }
     else{
         if(S_ISREG(buff.st_mode)){
             return(0);	//f
         }
         else if(S_ISDIR(buff.st_mode)){
             return(1);	//d
         }
         else if(S_ISCHR(buff.st_mode)){
             return(2);	//c
         }
         else if(S_ISBLK(buff.st_mode)){
             return(3);	//b
         }
         else if(S_ISLNK(buff.st_mode)){
             return(4);	//s
         }
         else{
             return(7);	// Unidentified file type
         }
     }
}


/**
  *@biref printing path(s)
  *@param path
  *@return none
  */

void printPath(char* path){
	printf("%s\n", path);

}


/**
  *@brief manipulating input string for convenience
  *@param s, c
  *@return void
  */
void replaceStr(char* s, char* c){
	for(int i=0; s[i] != '\0'; i++){
		if(s[i] != *c){
			continue;
                }
                if(strlen(s) != 1){
			if(s[strlen(s) - 1] == *c){
				s[strlen(s) -1] = '\0';
                         }
		}
          }
}
