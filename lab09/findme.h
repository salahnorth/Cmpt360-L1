/**
  *@file findme.h
  *@brief Header File
  *@author Salah Mohamed & Vy Tran
  *@date 27032024 
  *@version 8
  */

#ifndef FINDME_H
#define FINDME_H

#define DEFAULT_FILETYPE 5
#define NUM_THREADS 5

typedef struct{
    char* path;
    int filetype;
    char* filename;
    char* username;
    int maxdepth;
    
} threadArg;


// Function prototypes.

void *findFileThread(void *arg);
int findFile(char *path, int filetype,char* filename, char* username, int maxdepth);
int get_fileType(char *path, char* filename);
int fileSystemTests(char *filename);
void printPath(char*);
void replaceStr(char*, char*);
#endif


