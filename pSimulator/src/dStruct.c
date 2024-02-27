#include <stdio.h>
#include <stdlib.h>
#include "dStruct.h"

/**
  *@file dStruct.c
  *@brief This file performs the functionalities of the library
  *@author Salah Mohamed & Vy Tran
  *@date 26022024
  *@version 6
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
		entryList->head = entryList->head->next;
		free(temp);
	}
	free(entryList); //Free the entry list structure
}

/**@brief Creating a new entry
  *@param pid, status, niceness, cputime, proctime
  *@return Entry
  */
Entry* createEntry(int pid, int status, int niceness, float cputime, float proctime, float arrivaltime, float turnaroundtime, float responsetime, float cputimeSlice, int firstexecFlag, int indicator){
        Entry* newEntry = (Entry*)malloc(sizeof(Entry));

        if (newEntry != NULL) {
                newEntry->pid = pid;
                newEntry->status = status;
                newEntry->niceness = niceness;
                newEntry->cputime = cputime;
                newEntry->proctime = proctime;
                newEntry->arrivaltime = arrivaltime;
                newEntry->turnaroundtime = turnaroundtime;
                newEntry->responsetime = responsetime;
                newEntry->cputimeSlice = cputimeSlice;
                newEntry->firstexecFlag = firstexecFlag;
		newEntry->indicator = indicator;
                newEntry->next = NULL;

                return newEntry;
        } else {
                printf("Failed to create entry.\n");
                return NULL;
        }
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
void setEntry(EntryList* entryList, int pid, int status, int niceness, float cputime, float proctime){
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
void pushEntry(EntryList* entryList, Entry* newEntry){	
	// Check to see if allocating memory is successful and 
	//		if list can hold at most 100 entries
	if(newEntry != NULL){
		//Check if the list is empty
		if(entryList->size == 0){
			entryList->head    = newEntry;
			entryList->size++;
		}
		//Check if the list is less or equal to 100
		else if(entryList->size <= 100){

		// Set values for newEntry and place it to head of linked list
			//newEntry->next  = entryList->head;
			//entryList->head = newEntry;
			//entryList->size++;
			Entry* current = entryList->head;
			while (current != NULL){
				if (current->next == NULL){
					current->next = newEntry;
					newEntry->next = NULL;
				}
				current = current->next;
			}
			entryList->size ++;
		}
		else{
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
  *@return 
 */

Entry* popEntry(EntryList* entryList){

	// Check to see if the linked list is not empty to pop
	if(entryList->size != 0){
		Entry* searchEntry = entryList->head;
		entryList->head = entryList->head->next;
		entryList->size--;
		
		Entry* popEntry = copyEntry(searchEntry);
		
		return popEntry;

	}
	else{ 
		// Empty linked list
		printf("Unable to pop an empty linked list.\n");
		return NULL;
	}
}

/**@brief Copying an entry from the list
  *@param entry
  *@return copied entry
 */
Entry* copyEntry(Entry* originalEntry){

    // Copying the info of the original entry to our new one
    Entry* cpEntry = createEntry(originalEntry->pid, originalEntry->status,
		    originalEntry->niceness, originalEntry->cputime,
		    originalEntry->proctime, originalEntry->arrivaltime,
		    originalEntry->turnaroundtime, originalEntry->responsetime,
		    originalEntry->cputimeSlice, originalEntry->firstexecFlag,
		    originalEntry->indicator);
    
    return cpEntry;
}

/**@brief Getting the value of CPU time by pid
  *@param entryList, pid
  *@return cputime or -1.0
  */
float getCputime(EntryList* entryList, int pid){

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
void setCputime(EntryList* entryList, int pid, float cputime){

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

void resetCputimeSlice(Entry* entry){
    entry->cputimeSlice = 0.0;
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

/**@brief Swap entry values
  *@param 2 entries
  *@return none
  */
void swapEntries(Entry* entry1, Entry* entry2){
    int tempPid = entry1->pid;
    int tempStatus = entry1->status;
    int tempNiceness = entry1->niceness;
    float tempCputime = entry1->cputime;
    float tempProctime = entry1->proctime;
    float tempArrivaltime = entry1->arrivaltime;
    float tempTurnaroundtime = entry1->turnaroundtime;
    float tempResponsetime = entry1->responsetime;
    float tempCputimeSlice = entry1->cputimeSlice;
    float tempFirstexecFlag = entry1->firstexecFlag;
    
    entry1->pid = entry2->pid;
    entry1->status = entry2->status;
    entry1->niceness = entry2->niceness;
    entry1->cputime = entry2->cputime;
    entry1->proctime = entry2->proctime;
    entry1->arrivaltime = entry2->arrivaltime;
    entry1->turnaroundtime = entry2->turnaroundtime;
    entry1->responsetime = entry2->responsetime;
    entry1->cputimeSlice = entry2->cputimeSlice;
    entry1->firstexecFlag = entry2->firstexecFlag;
    
    entry2->pid = tempPid;
    entry2->status = tempStatus;
    entry2->niceness = tempNiceness;
    entry2->cputime = tempCputime;
    entry2->proctime = tempProctime;
    entry2->arrivaltime = tempArrivaltime;
    entry2->turnaroundtime = tempTurnaroundtime;
    entry2->responsetime = tempResponsetime;
    entry2->cputimeSlice = tempCputimeSlice;
    entry2->firstexecFlag = tempFirstexecFlag;
}

/**@brief Sort the entrylist based on Proctime
  *@param entryList
  *@return none
  */
void selectionSortByProctime(EntryList* entryList){
    if(entryList->head == NULL || entryList->size == 0){
        return;
    }
    Entry* current = entryList->head;
    
    while(current != NULL){
        Entry* minEntry = current;
        Entry* nextEntry = current->next;
        
        while(nextEntry != NULL){
            if(nextEntry->proctime < minEntry->proctime){
                minEntry = nextEntry;
            }
            nextEntry = nextEntry->next;
        }
        if(minEntry != current){
            swapEntries(current, minEntry);
        }
        current = current->next;
    }
}


