#include <stdio.h>
#include <stdlib.h>
#include "dStruct.h"

/**
  *@file dStruct.c
  *@brief This file performs the functionalities of the library
  *@author Salah Mohamed & Vy Tran
  *@date 18012024
  *@version 5
  */


/**@brief Initialization of a linked list
  *@param none
  *@return entryList
  */
EntryList* initialize(){ 
	
	//Allocating memory for the linked list
	EntryList* entryList = (EntryList*)malloc(sizeof(EntryList));

	//Check if memory allocation is successful
	if(entryList != NULL){
		entryList->head = NULL; //Initialize the head to NULL
		entryList->size = 0;	//Initialize the size to 0
	}
	return(entryList);
}

/**@brief Destroy the linked list/Free the memory
  *@param entryList
  *@return none
  */
void destroy(EntryList* entryList){
	//Traverse the list and free each entry
	while(entryList->head != NULL){
		Entry* temp = entryList->head;
		free(temp);
		entryList->head = entryList->head->next;
	}
	free(entryList); //Free the entry list structure
}

/**@brief Getting an entry in by pid
  *@param entryList, pid
  *@return current or NULL
  */
Entry* getEntry(EntryList* entryList, int pid, int index){
	Entry* current   = entryList->head;
	int currentIndex = 0;

	// Traverse the list to find the desired entry
	while(current != NULL){
		// Check if the current entry matches the specified PID/index
		if (current->pid == pid || currentIndex == index){
			// Return the entry if PID/index matches
			return(current); 
	
		}
		current = current->next;
		currentIndex++;
	}
	printf("\nUnable to get entry from pid or index.\n");
	return(NULL);
}

/**@brief Setting an entry
  *@param entryList, pid, status, niceness, cputime, proctime
  *@return none
  */
void setEntry(EntryList* entryList, int pid, int status, int niceness, double cputime, double proctime){
	Entry* entry = getEntry(entryList, pid, -1); 
	// Get the entry by pid. The -1 for index is to only search for pid

	// If entry is found, set the values
	if(entry != NULL){
		entry->status   = status;
		entry->niceness = niceness;
		entry->cputime  = cputime;
		entry->proctime = proctime;
	} 
	else{
		// If entry is not found
		printf("Entry not found. Cannot set values.\n\n");
	}
}

/**@brief Pushing an entry into a list (stack is implemented)
  *@param entryList, pid, status, niceness, cputime, proctime
  *@return none
 */ 
void pushEntry(EntryList* entryList, int pid, int status, int niceness, double cputime, double proctime){
	// Allocating memory for new entry
	Entry* newEntry = (Entry*)malloc(sizeof(Entry));	
	
	// Check to see if allocating memory is successful and 
	//		if list can hold at most 100 entries
	if(newEntry != NULL){
		//Check if the list is empty
		if(entryList->size == 0){
			entryList->head    = newEntry;
			newEntry->pid      = pid;
			newEntry->status   = status;
			newEntry->niceness = niceness;
			newEntry->cputime  = cputime;
			newEntry->proctime = proctime;
			entryList->size++;
		}
		//Check if the list is less or equal to 100
		else if(entryList->size <= 100){
		// Set values for newEntry and place it to head of linked list
			newEntry->pid      = pid;
			newEntry->status   = status;
			newEntry->niceness = niceness;
			newEntry->cputime  = cputime;
			newEntry->proctime = proctime;

			newEntry->next  = entryList->head;
			entryList->head = newEntry;
			entryList->size++;
		}
		else{
			printf("The Size exceeded 100.\n");
			return;

		}

	}
	else{
		// Failed to allocate memory
		printf("Failed to allocate memory for new entry.\n");
		return;
	}
}

/**@brief Popping an entry from the list
  *@param entryList
  *@return none
 */
void popEntry(EntryList* entryList){

	// Check to see if the linked list is not empty to pop
	if(entryList->size != 0){
		Entry* poppedEntry = entryList->head;
		entryList->head = entryList->head->next;
		free(poppedEntry);
		entryList->size--;

	}
	else{ 
		// Empty linked list
		printf("Unable to pop an empty linked list.\n");
	}
}

/**@brief Getting the value of CPU time by pid
  *@param entryList, pid
  *@return cputime or -1.0
  */
double getCputime(EntryList* entryList, int pid){

	//Get Entry by PID from entryList. 
	//Index is -1 because we're checking PID to get entry
	Entry* entry = getEntry(entryList, pid, -1);

	//Return the cputime from the given pid
	if(entry != NULL){
		return(entry->cputime);
	} 
	//Pid doesn't exist in the entryList
	printf("Unable to find CPU time\n");
	return(-1.0);
}

/**@brief Setting/Adjusting the value of CPU time by pid
  *@param entryList, pid, cputime
  *@return none
  */
void setCputime(EntryList* entryList, int pid, double cputime){

	//Get Entry by PID from entryList. 
	//Index is -1 because we're checking PID to get entry.
	Entry* entry = getEntry(entryList, pid, -1);

	//Set CPUTime if it exists
	if(entry != NULL){
		entry->cputime = cputime;
	} 
	else{
		printf("Unable to set Cputime\n");
	}
}

/**@brief Getting the value of status by pid
  *@param entryList, pid
  *@return status or -1.0
  */
int getStatus(EntryList* entryList, int pid){

	//Get Entry by PID from entryList. 
	//Index is -1 because we're checking PID to get entry.
        Entry* entry = getEntry(entryList, pid, -1);

	//Return the Status from the given pid
        if(entry != NULL){
                return(entry->status);
        } 
	// Pid doesn't exist in the entryList
        printf("Unable to find Status\n");
        return(-1);
}

/**@brief Setting/Adjusting the value of status by pid
  *@param entryList, pid, status
  *@return none
  */
void setStatus(EntryList* entryList, int pid, int status){

	//Get Entry by PID from entryList.
	//Index is -1 because we're checking PID to get entry
        Entry* entry = getEntry(entryList, pid, -1);

	//Set Status if it exists
        if(entry != NULL){
                entry->status = status;
        } 
	else{
                printf("Unable to set Status\n");
        }
}

/**@brief Getting the value of niceness by pid
  *@param entryList, pid
  *@return niceness or -1.0
  */
int getNiceness(EntryList* entryList, int pid){
	//Get Entry by PID from entryList. 
	//Index is -1 because we're checking PID to get entry
        Entry* entry = getEntry(entryList, pid, -1);

	//Return the Niceness from the given pid
        if(entry != NULL){
                return(entry->niceness);
        } 
	
	//Pid doesn't exist in the entryList.
        printf("Unable to find Niceness with corresponding pid\n");
        return(-1);
}

/**@brief Setting/Adjusting the value of niceness by pid
  *@param entryList, pid, niceness
  *@return none
  */
void setNiceness(EntryList* entryList, int pid, int niceness){
	//Get Entry by PID from entryList. 
	//Index is -1 because we're checking PID to get entry
        Entry* entry = getEntry(entryList, pid, -1);

	//Set Niceness if it exists
        if(entry != NULL){
                entry->niceness = niceness;
        } 
	else{
                printf("Unable to set niceness with corresponding pid\n");
        }
}

/**@brief Searching an entry by pid
  *@param entryList, pid
  *@return 1 or 0
  */
int searchEntryByPid(EntryList* entryList, int pid){

	//Get Entry by PID from entryList. 
	//Index is -1 because we're checking PID to get entry
	//If it does exists, return 1, if not, return 0
	if(getEntry(entryList, pid, -1) != NULL){
		return(1);
	}
	return(0);
}

/**@brief Searching an entry by niceness or status
  *@param entryList, status, niceness
  *@return 1 or 0
  */
int searchEntries(EntryList* entryList, int status, int niceness) {
	//Traverse the entryList until we find an entry with corresponding
	//status or niceness that matches with the given parameters
	Entry* current = entryList->head;
	while(current != NULL) {
		//Return 1 if found, else return 0
		if(current->niceness == niceness || current->status == status){
			return(1);
		}
		current = current->next;
	}
	return(0); //Entry not found
}

/**@brief Printing all entries
  *@param entryList
  *@return none
  */
void printAllEntries(EntryList* entryList){
	Entry* current = entryList->head; //Get the head of the linked list.
	int count=1;
	//If the linked list is empty, return
	if(current == NULL){
		printf("The Linked List is empty.\n");
		return;
	} 
	else{
		printf("//pid, status, niceness, cputime, proctime\n");
		//Traverse the linked list and print all the entries' info.
		while(current != NULL){
			printf("%d, %d, %d, %.1f, %.1f\n", current->pid, current->status, current->niceness, current->cputime, current->proctime);
			current = current->next;
			count++;
		}
	}
}

/**@brief Printing a specific entry by pid
  *@param entryList, pid
  *@return none
  */
void printEntryByPid(EntryList* entryList, int pid) {
	//Get Entry by PID from entryList. 
	//Index is -1 because we're checking PID to get entry
	Entry* entry = getEntry(entryList, pid, -1);

	//If entry exists, print all info of the entry, else print not found.
	if(entry != NULL){
		printf("//pid, status, niceness, cputime, proctime\n");
		printf("%d, %d, %d, %.1f, %.1f\n", entry->pid, entry->status, entry->niceness, entry->cputime, entry->proctime);
	} 
	else{
		printf("Entry with PID %d not found.\n", pid);
	}
}

/**@brief Printing a specific entry by niceness or status
  *@param entryList, status, niceness
  *@return none
  */
void printEntriesByNicenessStatus(EntryList* entryList, int status, int niceness) {
	Entry* current = entryList->head; // Get the head of the linked list
	printf("//pid, status, niceness, cputime, proctime\n");
	
	int count = 1;
	//Traverse the linked list
	while(current != NULL){
		// Print the entry's info if it matches status or niceness
		if(current->niceness == niceness || current->status == status){
			printf("%d, %d, %d, %.1f, %.1f\n",
					current->pid, current->status, current->niceness, current->cputime, current->proctime);
			count++;
		}
		current = current->next;
	}

	if(count == 1){
		printf("No entries found with the specified niceness and/or status.\n");
	}
}


