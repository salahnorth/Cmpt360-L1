/**
  *@file dStruct.h
  *@brief Header File
  *@author Salah Mohamed & Vy Tran
  *@date 18012024
  *@version 5
  */

#ifndef DSTRUCT_H
#define DSTRUCT_H

// Structure to represent an entry in the list
typedef struct Entry {
	int pid;
	int status;
	int niceness;
	double cputime;
	double proctime;
	struct Entry* next;
} Entry;

// Structure to represent a list of entries
typedef struct EntryList {
	Entry* head;
	int size;
} EntryList;

// Function prototypes.

EntryList* initialize();

void destroy(EntryList* entryList);

Entry* createEntry(int pid, int status, int niceness, double cputime, double proctime);

Entry* getEntry(EntryList* entryList, int pid, int index);

void setEntry(EntryList* entryList, int pid, int status, int niceness, double cputime, double proctime);

void pushEntry(EntryList* entryList, Entry* newEntry);

Entry* popEntry(EntryList* entryList);

double getCputime(EntryList* entryList, int pid);

void setCputime(EntryList* entryList, int pid, double cputime);

int getStatus(EntryList* entryList, int pid);

void setStatus(EntryList* entryList, int pid, int status);

int getNiceness(EntryList* entryList, int pid);

void setNiceness(EntryList* entryList, int pid, int niceness);

int searchEntryByPid(EntryList* entryList, int pid);

int searchEntries(EntryList* entryList, int status, int niceness);

void printAllEntries(EntryList* entryList);

void printEntryByPid(EntryList* entryList, int pid);

void printEntriesByNicenessStatus(EntryList* entryList, int status, int niceness);

#endif
