#include<dirent.h>
#include<unistd.h>
#include<getopt.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>
#include<stdio.h>

/**
   *@brief Reading the input files to extract proctime and niceness values
   *@param path, proctime, niceness
   *@return 1 or 0
   */
 int readFromFile(const char *path, int filetype, int maxdepth,char* username,
		 char* filename){
 	 struct dirent *entry;
         DIR *dp;
	
	printf("PATH %s\n", path);
	printf("FILE NAME %s\n", filename);	
         dp = opendir(path);
         if(dp == NULL){
                 perror("opendir");
                 return(0);
         }
 
         while((entry = readdir(dp))){
                  FILE *fp;
 
                  if(entry->d_name[0] == '.'){
                         continue;
                  }
                  char *filePath = (char*) malloc(strlen(path) + strlen(entry->d_name));
 
                  filePath = strcpy(filePath, path);
                  strcat(filePath, entry->d_name);
 
                  if(filetype == 0){
                         fp = fopen(filePath, "r");
                          if(fp == NULL){
                                 printf("Unable to read file\n");
			  }
			  else{
			 	 if(strcmp(entry->d_name, filename) == 0){
					  printf("MATCHED! File name from filename %s\n",
						 filename);
				  //printf("File name from entry %s\n", entry->d_name);
				  }
			

				  free(filePath);
			 	 //return(1);
			  }
		  }
	 }


          closedir(dp);
          //return(0);
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
             return(-1);
     }
     else{
         if(S_ISREG(buff.st_mode)){
             return(0);
         }
         else if(S_ISDIR(buff.st_mode)){
             return(1);
         }
         else if(S_ISCHR(buff.st_mode)){
             return(2);
         }
         else if(S_ISBLK(buff.st_mode)){
             return(3);
         }
         else if(S_ISFIFO(buff.st_mode)){
             return(4);
         }
         else if(S_ISLNK(buff.st_mode)){
             return(5);
         }
         else if(S_ISSOCK(buff.st_mode)){
             return(6);
         }
         else{
             return(7);
         }
     }
}


int main(int argc, char *argv[]){
	if (argc < 1){
		printf("ERROR!\n");
		exit(1);

	}
	else{
		char* path = "../lab09/";
		char* fname = "findme.c";
		int ftype;
		ftype = fileSystemTests(fname);
		printf("file type %i\n", ftype);
		readFromFile(path,ftype, 5, "chipcaptain", fname);
	}
	



}
