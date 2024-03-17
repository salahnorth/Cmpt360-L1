#include<dirent.h>
#include<unistd.h>
#include<getopt.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>
#include<stdio.h>
#include<pwd.h>


/**
   *@brief Reading the input files to extract proctime and niceness values
   *@param path, proctime, niceness
   *@return 1 or 0
   */
int readFromFile(const char *path, int filetype, int maxdepth,char* username,
		 char* filename){
 	 struct dirent *entry;
         DIR *dp;
 	/*char c1 = 'f';
	char* cpt1;
	cpt1 = &c1;

	char c2 = 'd';
	char* cpt2;
	cpt2 = &c2;*/

	printf("PATH %s\n", path);
	printf("FILE NAME %s\n", filename);
	printf("USERNAME OUTSIDE LOOP %s\n", username);
	printf("FILE TYPE OUTSIDE LOOP %i\n", filetype);	
	struct stat statbuf;
	struct passwd *pwd;

	//pwd = getpwuid(statbuf.st_uid);
	//printf("USERNAME 2 %s\n", pwd->pw_name);
	
		dp = opendir(path);
		if(dp == NULL){
        	         perror("opendir");
                	 return(0);
		}
		
		int counter = 0;
		//int localMaxdepth;
		//localMaxdepth = maxdepth;

		//printf("BEFORE THE LOOP; LOCAL MAXDEPTH %i\n", localMaxdepth);
		printf("BEFORE THE LOOP; MAXDEPTH %i\n", maxdepth);

		//int temp;

         	while((entry = readdir(dp))){
			FILE *fp;
			//temp = lstat(dp->name, statbuf);
			if (lstat(entry->d_name, &statbuf) == -1)
        			continue;
 			//if((pwd = getpwuid(statbuf.st_uid)) != NULL){
	//			 printf("USER NAME INSIDE LOOP %s\n", pwd->pw_name);
			 //}
			 /*else{
				 printf("UID  %d\n", statbuf.st_uid);
			 }*/
			 if(entry->d_name[0] == '.'){
                         	continue;
			 }
                  	char *filePath = (char*) malloc(strlen(path) + strlen(entry->d_name));
 
                  	filePath = strcpy(filePath, path);
                  	strcat(filePath, entry->d_name);
		  	if(maxdepth > 0){ 
				//if(filetype == 1){
				//	counter++;
		//			if(counter < maxdepth){
						if(filetype == 0){
							fp = fopen(filePath, "r");
							if(fp == NULL){
								printf("Unable to read file\n");
					 		}
					 		else{
								if(strcmp(entry->d_name, filename) == 0){
									printf("MATCHED! File name from filename %s\n", filename);
				  //printf("File name from entry %s\n", entry->d_name);
				  				
								}
								free(filePath);
			 	 //return(1)
			  				}

				 		//}	
					}
		 		//}
			 	//localMaxdepth--;
				maxdepth--;
				//printf("THE END OF LOOP; LOCAL MAXDEPTH %i\n",
				//		localMaxdepth);
				printf("THE END OF LOOP; MAXDEPTH %i\n",
						maxdepth);
				printf("THE END OF LOOP; COUNTER %i\n", counter);
			 	readFromFile(path,filetype,maxdepth,username,filename);
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


int main(int argc, char *argv[]){
	if (argc < 1){
		printf("ERROR!\n");
		exit(1);

	}
	else{
		char* path = "../lab09/";// back to ~/360lab/lab09/findme.c
		char* fname = "findme.c";
		int ftype;
		//char* fpt;
		//fpt=&ftype;
		ftype = fileSystemTests(fname);
		printf("file type %i\n", ftype);
		readFromFile(path,ftype, 5, "chipcaptain", fname);
	}
	
}
