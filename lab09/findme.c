#include<dirent.h>
#include<unistd.h>
#include<getopt.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>
#include<stdio.h>
#include<pwd.h>



/*

*/

int COUNTER2 = 0;
int counter = 0;

int readFromFile(char *path, int filetype,char* filename,char* username, int maxdepth){
	COUNTER2 = COUNTER2 + 1;    
    struct dirent *entry;
    DIR *dp;
    /*char c1 = 'f';
    char* cpt1;
    cpt1 = &c1;

    char c2 = 'd';
    char* cpt2;
    cpt2 = &c2;*/

/*
    printf("PATH %s\n", path);
    printf("FILE NAME %s\n", filename);
    printf("USERNAME OUTSIDE LOOP %s\n", username);
    printf("FILE TYPE OUTSIDE LOOP %i\n", filetype);
    */	
    
    struct stat statbuf;
    struct passwd *pwd;
    
    //pwd = getpwuid(statbuf.st_uid);
    //printf("USERNAME 2 %s\n", pwd->pw_name);
    
    dp = opendir(path);
    
    if(dp == NULL){
        perror("opendir");
        return(0);
    }
    
    //int counter = 0;
   
    int localMaxdepth;
    localMaxdepth = maxdepth;

    //printf("BEFORE THE LOOP; LOCAL MAXDEPTH %i\n", localMaxdepth);
    //printf("BEFORE THE LOOP; MAXDEPTH %i\n", maxdepth);
    
    //int temp;
	printf("\n\nCOUNTER2 is %i\n\n", COUNTER2); 	    
    while((entry = readdir(dp))){
    
        FILE *fp1;
        //temp = lstat(dp->name, statbuf);
        
        if (lstat(path, &statbuf) == -1)
            continue;
            
            /*if((pwd = getpwuid(statbuf.st_uid)) != NULL){
                  printf("USER NAME INSIDE LOOP %s\n", pwd->pw_name);
	     }
	      else{
	          printf("UID  %d\n", statbuf.st_uid);
	      }*/
	if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
	    continue;
	}
	
	char *filePath = (char*) malloc((strlen(path)+1) + (strlen(entry->d_name)+1));
	
	/*char c = '/';
	char* pt;
	pt = &c;*/
		
	filePath = strcpy(filePath, path);
	strcat(filePath, "/");
	strcat(filePath, entry->d_name);
	
	//printf("\nPath is %s\n", filePath);
	
	int file_type = fileSystemTests(filePath);
	//printf("The FILETYPE FROM FUNCTION is %d\n", file_type);
	printf("FILE PATH BEFORE IF MAXDEPTH %s\n", filePath);
	if (maxdepth > 0){
		//printf("MAXDEPTH INSIDE %i\n", maxdepth);		
	    if(file_type == 1){
		//printf("FOUND A DIRECTORY\n");
		//strcat(filePath, "/");
		printf("FILEPATH AFTER FINDING DIRECTORY %s\n", filePath);

	//	filePath = strcpy(filePath, "./dir1/dir1.1/");
		localMaxdepth--;
		readFromFile(filePath,filetype,filename, username, localMaxdepth);
	  	printf("MAXDEPTH AFTER CALLING FUNCTION AGAIN %i\n",localMaxdepth);
		printf("FILEPATH UPDATED %s\n", filePath);


	    }
	
	    /*if(filetype == 1){
	          counter++;
	          if(counter < maxdepth){*/
		printf("FILE TYPE INPUT %i\n", filetype);
		printf("FILE WITH DASH %i\n", file_type);
	   	printf("FILEPATH BEFORE GOING INTO IF filetype == 0 %s\n",filePath); 
	    if(filetype == file_type){
		printf("GOING INSIDE IF\n");
		fp1 = fopen(filePath, "r");
		printf("ENTRY FILE NAME INSIDE IF %s\n", entry->d_name);
		printf("FILEPATH AFTER GOING INSIDE IF %s\n", filePath);
	        if(fp1 == NULL){
	            printf("Unable to read file\n");
	        }
	        else{
			//printf("ENTRY NAME %s\n", entry->d_name);
			//printf("GOING INTO ELSE\n");
			//printf("USER NAME INSIDE ELSE %s\n", filename);
	
	            if((strcmp(entry->d_name, filename) == 0) || filename == "DEFAULT");{
	
	                pwd = getpwuid(statbuf.st_uid);
		     printf("\n\n\n\n\n USER NAME IS %s\n\n\n\n\n", pwd->pw_name);	
	                if (pwd != NULL && strcmp(username, pwd->pw_name) ==
			0){
	                    printf("MATCHED! Path is %s\n", filePath);
	                    //printf("The MAXDEPTH IS %d\n", localMaxdepth);
			   printPath(filePath);
			    free(filePath);
				//return(1);
			}
			else{
				printf("WRONG USERNAME\n");
			}
			    //printf("YAYA\n");
	            }
		    //else{
		//	free(filePath);
		  //  }


		
		    //else{
		//	    printf("CAN'T FIND ANYTHING\n");
	            //free(filePath);
		    //printf("PATH AFTER %s\n", filePath);
		    //readFromFile(filePath,filetype, filename, username, maxdepth);

	        //}
	        
	    }
	        //maxdepth--;
	        //printf("THE END OF LOOP; MAXDEPTH %i\n", maxdepth);
	        //printf("THE END OF LOOP; COUNTER %i\n", counter);
	        //readFromFile(path,filetype,maxdepth,username,filename);
        }


	}
	counter++;
	printf("\n\nROUND NUMBER %i\n\n", counter);
	//free(filePath);
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

void printPath(char* path){
	printf("PRINTING PATH %s\n", path);

}
