/**
  *@file findme.h
  *@brief Header File
  *@author Salah Mohamed & Vy Tran
  *@date 
  *@version 
  */

#ifndef FINDME_H
#define FINDME_H

// Function prototypes.

int findFile(char *path, int filetype,char* filename, char* username, int maxdepth);
int get_fileType(char *path, char* filename);
int fileSystemTests(char *filename);
void printPath(char*);
#endif
