/**
  *@file findme.h
  *@brief Header File
  *@author Salah Mohamed & Vy Tran
  *@date 02042024 
  *@version 11
  */

#ifndef FINDME_H
#define FINDME_H

#define DEFAULT_FILETYPE 5


// Function prototypes.

int findFile(char *path, int filetype,char* filename, char* username, int maxdepth);
int get_fileType(char *path, char* filename);
int fileSystemTests(char *filename);
void printPath(char*);
void replaceStr(char*, char*);

#endif


