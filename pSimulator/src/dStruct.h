/**
  *@file dStruct.h
  *@brief Header File
  *@author Salah Mohamed & Vy Tran
  *@date 26 February 2024
  *@version 6
  */

#ifndef DSTRUCT_H
#define DSTRUCT_H

// Structure to represent an entry in the list
typedef struct Entry {
	int pid;
	int status;
	int niceness;
	float cputime;
	float proctime;
	float arrivaltime;
	float turnaroundtime;
	float responsetime;
	float cputimeSlice;
	int firstexecFlag;
	int indicator;
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

Entry* createEntry(int pid, int status, int niceness, float cputime, float
		proctime, float arrivaltime, float turnaroundtime, float
		responsetime, float cputimeSlice, int firstexecFlag, int
		indicator);

Entry* getEntry(EntryList* entryList, int pid, int index);

void setEntry(EntryList* entryList, int pid, int status, int niceness, float cputime, float proctime);

void pushEntry(EntryList* entryList, Entry* newEntry);

Entry* popEntry(EntryList* entryList);

Entry* copyEntry(Entry* originalEntry);

float getCputime(EntryList* entryList, int pid);

void setCputime(EntryList* entryList, int pid, float cputime);

int getStatus(EntryList* entryList, int pid);

void setStatus(EntryList* entryList, int pid, int status);

int getNiceness(EntryList* entryList, int pid);

void setNiceness(EntryList* entryList, int pid, int niceness);

void resetCputimeSlice(Entry* entry);

int searchEntryByPid(EntryList* entryList, int pid);

int searchEntries(EntryList* entryList, int status, int niceness);

void printAllEntries(EntryList* entryList);

void printEntryByPid(EntryList* entryList, int pid);

void printEntriesByNicenessStatus(EntryList* entryList, int status, int niceness);

void swapEntries(Entry* entry1, Entry* entry2);

void selectionSortByProctime(EntryList* entryList);

#endif
