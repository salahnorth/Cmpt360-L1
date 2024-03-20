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

int readFromFile(char *path, int filetype,char* filename, char* username, int maxdepth);
int fileSystemTests(char *filename);
void printPath(char*);
#endif
