#include <stdio.h>
#include <stdlib.h>
#include "dStruct.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define READY 1
#define RUNNING 2
#define COMPLETE 3

#define ALGOR_FIFO 1
#define ALGOR_SJF 2
#define ALGOR_RR 3
#define ALGOR_MLFQ 4

#define TIME_JIFFY 2
#define TIME_DT 0.1

#define TIME_RESET 20.0

#define FAIL_READ_FILE 0

#define SOURCE_DIR "../newProc/"
#define BACKUP_DIR "../backup_newProc/"


/**
  *@file procSim.c
  *@brief This file sets up a process simulator
  *@author Salah Mohamed & Vy Tran
  *@date 04022024
  *@version 9
 */

int currentPID = 0;
float globalTime = 0.0f;
int fileNumber = 1;
float quantumCounter = 0.0f;
//float deltaCputime = 0.1;


/**
  *@brief Check if the file is regular
  *@param filePath
  *@return 1 or 0
  */

int isRegularFile(char* filePath) {
    struct stat fileStat;

    if(stat(filePath, &fileStat) == 0){
        if(S_ISREG(fileStat.st_mode)){
            return(1);
        } 
	else{
            return(0);
        }
    }
    return(0);
}

/**
  *@brief Move file to backup directory
  *@param fileName
  *@return 1 if successful, 0 otherwise
  */
int moveFileToBackup(char* fileName) {

    

    char sourcePath[256];
    char backupPath[256];
    
    strcpy(sourcePath, SOURCE_DIR);
    strcat(sourcePath, fileName);
    
    strcpy(backupPath, BACKUP_DIR);
    strcat(backupPath, fileName);

    if(rename(sourcePath, backupPath) == 0) {
        printf("Moved file %s to backup.\n", fileName);
        return 1;
    } else {
        perror("Error moving file to backup");
        return 0;
    }
}

/**
  *@brief Move files from backup directory to source directory
  *@return 1 if successful, 0 otherwise
  */
int moveFilesFromBackup() {
    DIR *dp;
    struct dirent *entry;

    dp = opendir(BACKUP_DIR);
    if(dp == NULL) {
        perror("Error opening backup directory");
        return 0;
    }

    while((entry = readdir(dp))) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char sourcePath[256];
            char backupPath[256];
            
            strcpy(sourcePath, SOURCE_DIR);
            strcat(sourcePath, entry->d_name);
            
            strcpy(backupPath, BACKUP_DIR);
            strcat(backupPath, entry->d_name);
            
            if(rename(backupPath, sourcePath) != 0) {
                perror("Error moving file from backup to source");
                return 0;
            }
        }
    }

    closedir(dp);
    return 1;
}

/**
  *@brief Reading the input files to extract proctime and niceness values
  *@param path, proctime, niceness
  *@return 1 or 0
  */

int readFromFile(const char *path, float *proctime, int *niceness){
	struct dirent *entry;
 	DIR *dp;
	
	dp = opendir(path);
  	if(dp == NULL){
		perror("opendir");
		return(0);
       	}
	
	while((entry = readdir(dp))){
		FILE *fp;
		
		if(entry->d_name[0] == '.'){
			continue;
       	 	}

		char *filePath = (char*) malloc(strlen(path) + strlen(entry->d_name)); 
	
		filePath = strcpy(filePath, path);
		strcat(filePath, entry->d_name);
		
		if(isRegularFile(filePath) == 1){
			fp = fopen(filePath, "r");
			
			if(fp == NULL){
				printf("Unable to read file\n");
			}
			else{
                   		fscanf(fp, "%f %d", proctime, niceness);
                    		printf("Read from file %s: Proctime: %.2f, Niceness: %d\n", entry->d_name, *proctime, *niceness);
                    		fclose(fp);
				// Deleting the file after reading

				if(moveFileToBackup(entry->d_name) != 1){
                	        	perror("Error moving file to backup\n");
			    	} 

                    		free(filePath);
                    		return(1);
           		 }
        	}
    	}
	closedir(dp);
	return(0);                       
}

int setPriority(int niceness) {
    int priority = 0;

    if (niceness > 80) {
        priority = 5;
    } else if (niceness > 60 && niceness <= 80) {
        priority = 4;
    } else if (niceness > 40 && niceness <= 60) {
        priority = 3;
    } else if (niceness > 20 && niceness <= 40) {
        priority = 2;
    } else if (niceness < 20) {
        priority = 1;
    }

    return priority;
}



/**
  *@brief Print all the entries to a file
  *@param file, EntryList
  *@return none
  */
  
void printAllEntriesToFile(FILE* file, EntryList* entryList){

    Entry* current = entryList->head;

    while(current != NULL){
        fprintf(file, "%.2f, %d, %d, %d, %.2f, %.2f, %.2f, %.2f, %.2f, %d, %d, %d\n",
                globalTime, current->pid, current->status, current->niceness,
                current->cputime, current->cputimeSlice, current->proctime,
		current->turnaroundtime, current->responsetime,
		current->indicator, current->originalPriority, current->currentPriority);
        current = current->next;
    }

}

int emptyQueues(EntryList* queue1, EntryList* queue2, EntryList* queue3, EntryList* queue4, EntryList* queue5){
    if (queue1->size == 0 && queue2->size == 0 && queue3->size == 0 && queue4->size == 0 && queue5->size == 0){
        return 0;
    }
    return 1;
}

/**
  *@brief Log the state of queues to a file
  *@param logFile, readyQueue, runningQueue
  *@return none
  */
void logState(int type, FILE* logFile, EntryList* readyQueue, EntryList* queue1, EntryList* queue2, EntryList* queue3, EntryList* queue4, EntryList* queue5, EntryList* runningQueue, EntryList* completeQueue){

    if (type == 4){
        printAllEntriesToFile(logFile, queue1);
        fprintf(logFile, "\n");
        printAllEntriesToFile(logFile, queue2);
        fprintf(logFile, "\n");
        printAllEntriesToFile(logFile, queue3);
        fprintf(logFile, "\n");
        printAllEntriesToFile(logFile, queue4);
        fprintf(logFile, "\n");
        printAllEntriesToFile(logFile, queue5);
        fprintf(logFile, "\n");
        
        printAllEntriesToFile(logFile, runningQueue);
        fprintf(logFile, "\n\n");
        printAllEntriesToFile(logFile, completeQueue);
        fprintf(logFile, "\n\n\n");
    }

    
    
    else{
	    printAllEntriesToFile(logFile, readyQueue);
	    fprintf(logFile, "\n");
	    printAllEntriesToFile(logFile, runningQueue);
	    fprintf(logFile, "\n\n");
	    printAllEntriesToFile(logFile, completeQueue);
	    fprintf(logFile, "\n\n\n");
    }
}




/**
  *@brief Simulate the process executiong cycle
  *@param none
  *@return none
  */
  
void FIFOProcessSimulator(){
    // Initializing the queues
    EntryList* readyQueue    = initialize();
    EntryList* runningQueue  = initialize();
    EntryList* completeQueue = initialize();
    
    EntryList* queue1 = NULL;
    EntryList* queue2 = NULL;
    EntryList* queue3 = NULL;
    EntryList* queue4 = NULL;
    EntryList* queue5 = NULL;
    
    // Initially set the time variables to 0 (start of global time) 
    float cputime = globalTime;
    float arrivaltime = globalTime;
    float turnaroundtime = globalTime;
    float responsetime = globalTime;
    float cputimeSlice = globalTime;
    
    int firstexecflag = 0; // Flag to indicate if a process is executing for the first time
    
    int indicator = 0;	// Process is changed inside the running queue
    
    // No need for priority for this algorithm
    int originalPriority = 0;
    int currentPriority = 0;

    // Open log files to record simulated results
    FILE* logFile = fopen("../log/FIFOLog.txt", "w");
    if(logFile == NULL){
        printf("Error opening log file.\n");
        return;
    }
    
    printf("FIFO Simulation started.\n");

    do{

        // Read from files and add processes to readyQueue
        int niceness;
        float proctime;
        char* path = "../newProc/";
        if(readFromFile(path, &proctime, &niceness) != FAIL_READ_FILE){
        
            /// Create a new process entry and add it to readyQueue
            Entry* newEntry = createEntry(++currentPID, READY, niceness, cputime, proctime, arrivaltime, turnaroundtime, responsetime, cputimeSlice, firstexecflag, indicator, originalPriority, currentPriority);
            pushEntry(readyQueue, newEntry);
            newEntry->arrivaltime += globalTime; // Increment arrivaltime for the next process
        }
         	
   
        // Process all ready processes
        if(runningQueue->size == 0){
        
            // Fetch the process from the readyQueue if the runningQueue is empty, then execute it
	    Entry* readyProcess = popEntry(readyQueue);
	    readyProcess->responsetime = globalTime - readyProcess->arrivaltime; // Calculate response time
		
		if(readyProcess != NULL){
		
		    // Calculate response time for the process if its their first time exectuing
                    if (readyProcess->firstexecFlag == 0){
		        readyProcess->responsetime = globalTime - readyProcess->arrivaltime; // Calculate response time
		        readyProcess->firstexecFlag += 1; // Set the flag to 1 to indicate first execution
		    }
		
		    readyProcess->status = RUNNING; // Move to running state
		    pushEntry(runningQueue, readyProcess);
            	} 
        }

        
         

        // Update running processes
        Entry* runningProcess = runningQueue->head;

        if(runningProcess != NULL){
        
	    runningProcess->cputime += (float) TIME_DT;
	    runningProcess->cputimeSlice += (float) TIME_DT;
		
	    // Check if the process has completed execution
	    if(runningProcess->cputime >= runningProcess->proctime){
	    
	        // If the process is complete, move it to the completeQueue
	        runningProcess->turnaroundtime = globalTime - runningProcess->arrivaltime; // Calculate turnaround time
	        runningProcess->cputime = runningProcess->proctime; // Completed process execution
        	
        	resetCputimeSlice(runningProcess); // Reset cputimeSlice to 0 in order to track the time in the running queue before popped
	        runningProcess->indicator = 1;
	        Entry* completeProcess = popEntry(runningQueue);
		completeProcess->status = COMPLETE;
		
		pushEntry(completeQueue, completeProcess);
		

            }
            
        }
        
        // Increment the global time for each cycle
        globalTime += (float) TIME_DT;
        
        // Log the state of queues to the file
        logState(1, logFile, readyQueue, queue1, queue2, queue3, queue4, queue5, runningQueue, completeQueue);
        
        

    }while(!(readyQueue->size == 0 && runningQueue->size == 0));  

    printf("\nFIFO Simulation completed.\n");
    printf("End of simulation loop. Ready queue size: %d, Running queue size: %d\n\n", readyQueue->size, runningQueue->size);
    
    // Close log file and clean up memory
    fclose(logFile);
    destroy(readyQueue);
    destroy(runningQueue);
    destroy(completeQueue);
}

void SJFProcessSimulator(){

    // Initializing the queues
    EntryList* readyQueue    = initialize();
    EntryList* runningQueue  = initialize();
    EntryList* completeQueue = initialize();
    
    EntryList* queue1 = NULL;
    EntryList* queue2 = NULL;
    EntryList* queue3 = NULL;
    EntryList* queue4 = NULL;
    EntryList* queue5 = NULL;
    
    // Initially set the time variables to 0 (start of global time) 
    float cputime = globalTime;
    float arrivaltime = globalTime;
    float turnaroundtime = globalTime;
    float responsetime = globalTime;
    float cputimeSlice = globalTime;
    
    int firstexecflag = 0; // Flag to indicate if a process is executing for the first time
    
    int indicator = 0;

    // No need for priority for this algorithm
    int originalPriority = 0;
    int currentPriority = 0;
    
    // Open log files to record simulated results
    FILE* logFile = fopen("../log/SJFLog.txt", "w");
    if(logFile == NULL){
        printf("Error opening log file.\n");
        return;

    }

    
    printf("SJF Simulation started.\n");
   

    do{

        // Read from files and add processes to readyQueue
        int niceness;
        float proctime;
        char* path = "../newProc/";
        while(readFromFile(path, &proctime, &niceness) != FAIL_READ_FILE){
        
            /// Create a new process entry and add it to readyQueue
            Entry* newEntry = createEntry(++currentPID, READY, niceness, cputime, proctime, arrivaltime, turnaroundtime, responsetime, cputimeSlice, firstexecflag, indicator, originalPriority, currentPriority);
            pushEntry(readyQueue, newEntry);
            newEntry->arrivaltime += globalTime; // Increment arrivaltime for the next process
            selectionSortByProctime(readyQueue); // Sort the ready queue in ascending order for SJF algorithm
            globalTime += TIME_DT;
            
        }
         	
   
        // Process all ready processes
        if(runningQueue->size == 0){
        
            // Fetch the process from the readyQueue if the runningQueue is empty, then execute it
	    Entry* readyProcess = popEntry(readyQueue);
	    readyProcess->responsetime = globalTime - readyProcess->arrivaltime; // Calculate response time
		
		if(readyProcess != NULL){
		
		    // Calculate response time for the process if its their first time exectuing
                    if (readyProcess->firstexecFlag == 0){
		        readyProcess->responsetime = globalTime - readyProcess->arrivaltime; // Calculate response time
		        readyProcess->firstexecFlag += 1; // Set the flag to 1 to indicate first execution
		    }
		
		    readyProcess->status = RUNNING; // Move to running state
		    pushEntry(runningQueue, readyProcess);
            	} 
        }
        
         

        // Update running processes
        Entry* runningProcess = runningQueue->head;

        if(runningProcess != NULL){
        
	    runningProcess->cputime += (float) TIME_DT;
	    runningProcess->cputimeSlice += (float) TIME_DT;
	
	    // Check if the process has completed execution
	    if(runningProcess->cputime >= runningProcess->proctime){
	    
	        // If the process is complete, move it to the completeQueue
	        runningProcess->turnaroundtime = globalTime - runningProcess->arrivaltime; // Calculate turnaround time
	        runningProcess->cputime = runningProcess->proctime; // Completed process execution
                
                resetCputimeSlice(runningProcess); // Reset cputimeSlice to 0 in order to track the time in the running queue before popped
	        
	        Entry* completeProcess = popEntry(runningQueue);
		completeProcess->status = COMPLETE;
		
		pushEntry(completeQueue, completeProcess);
		

            }
            
        }
        
        // Increment the global time for each cycle
        globalTime += (float) TIME_DT;
        
        // Log the state of queues to the file
        logState(2, logFile, readyQueue, queue1, queue2, queue3, queue4, queue5, runningQueue, completeQueue);


    }while(!(readyQueue->size == 0 && runningQueue->size == 0));

    printf("\nSJF Simulation completed.\n");
    printf("End of simulation loop. Ready queue size: %d, Running queue size: %d\n\n", readyQueue->size, runningQueue->size);
    
    // Close log file and clean up memory
    fclose(logFile);
    destroy(readyQueue);
    destroy(runningQueue);
    destroy(completeQueue);
}

void roundRobinProcessSimulator(){

    // Initializing the queues
    EntryList* readyQueue    = initialize();
    EntryList* runningQueue  = initialize();
    EntryList* completeQueue = initialize();
    
    EntryList* queue1 = NULL;
    EntryList* queue2 = NULL;
    EntryList* queue3 = NULL;
    EntryList* queue4 = NULL;
    EntryList* queue5 = NULL;
    
    // Initially set the time variables to 0 (start of global time) 
    float cputime = globalTime;
    float arrivaltime = globalTime;
    float turnaroundtime = globalTime;
    float responsetime = globalTime;
    float cputimeSlice = globalTime;
    
    int firstexecflag = 0; // Flag to indicate if a process is executing for the first time
    float quantumCounter = 0.0f; // Counter for quantum comparison
    
    
    
    int indicator = 1;
    
    // No need for priority for this algorithm
    int originalPriority = 0;
    int currentPriority = 0;

    // Open log files to record simulated results
    FILE* logFile = fopen("../log/roundrobinLog.txt", "w");
    if(logFile == NULL){
        printf("Error opening log file.\n");
        return;
    }
    
    
    printf("Round robin Simulation started.\n");

    do{

        // Read from files and add processes to readyQueue
        int niceness;
        float proctime;
        char* path = "../newProc/";
        
        if(readFromFile(path, &proctime, &niceness) != FAIL_READ_FILE){
        
            // Create a new process entry and add it to readyQueue
            Entry* newEntry = createEntry(++currentPID, READY, niceness, cputime, proctime, arrivaltime, turnaroundtime, responsetime, cputimeSlice, firstexecflag, indicator, originalPriority, currentPriority);
            pushEntry(readyQueue, newEntry);
            newEntry->arrivaltime += globalTime; // Increment arrivaltime for the next process
        }
         	
        // Process all ready processes
        if(runningQueue->size == 0){
        
            // Fetch the process from the readyQueue if the runningQueue is empty, then execute it
	    Entry* readyProcess = popEntry(readyQueue);
	    if(readyProcess != NULL){
		
		// Calculate response time for the process if its their first time exectuing
                if (readyProcess->firstexecFlag == 0){
		    readyProcess->responsetime = globalTime - readyProcess->arrivaltime; // Calculate response time
		    readyProcess->firstexecFlag = 1; // Set the flag to 1 to indicate first execution
		}

		readyProcess->status = RUNNING; // Move to running state
		pushEntry(runningQueue, readyProcess);
            } 
        }
        
        // Increment the global time for each cycle
        globalTime += TIME_DT; 

        // Update running processes
        Entry* runningProcess = runningQueue->head;

        if(runningProcess != NULL){
	    runningProcess->cputime += TIME_DT;
	    quantumCounter += TIME_DT;
	    runningProcess->cputimeSlice += TIME_DT;
		
	    // Check if the process has completed execution
	    if(runningProcess->cputime >= runningProcess->proctime){
		
		// If the process is complete, move it to the completeQueue
		runningProcess->cputime = runningProcess->proctime; // Completed process execution
		
		runningProcess->turnaroundtime = globalTime - runningProcess->arrivaltime; // Calculate turnaround time
	 	// Log the state of queues to the file
                //logState(3, logFile, readyQueue, queue1, queue2, queue3, queue4, queue5, runningQueue, completeQueue);
                
                printf("PID = %d has completed at %.2f\n", runningProcess->pid, runningProcess->cputime);
                	
		resetCputimeSlice(runningProcess); // Reset cputimeSlice to 0 in order to track the time in the running queue before popped
		
	        Entry* completeProcess = popEntry(runningQueue);
		completeProcess->status = COMPLETE;
		
		pushEntry(completeQueue, completeProcess);
		
		logState(3, logFile, readyQueue, queue1, queue2, queue3, queue4, queue5, runningQueue, completeQueue);
		
	      // If quantum time elapsed and process is not complete, preempt the process
            } else if(quantumCounter > TIME_JIFFY && runningProcess->cputime < runningProcess->proctime){
        	// Log the state of queues to the file
                logState(3, logFile, readyQueue, queue1, queue2, queue3, queue4, queue5, runningQueue, completeQueue);      	 
                
                resetCputimeSlice(runningProcess); // Reset cputimeSlice to 0 in order to track the time in the running queue before popped
               	Entry* preemptedProcess = popEntry(runningQueue);  
            	preemptedProcess->status = READY;  // Move the process back to the readyQueue
          
            	pushEntry(readyQueue, preemptedProcess);
            	
            	quantumCounter = 0; // Reset the quantum counter
		preemptedProcess->indicator++;

            }
	    else{
		// Log the state of queues to the file
                logState(3, logFile, readyQueue, queue1, queue2, queue3, queue4, queue5, runningQueue, completeQueue);
	    }
        
	}	

    }while(!(readyQueue->size == 0 && runningQueue->size == 0));

    printf("\nRound Robin Simulation completed.\n");
    printf("End of simulation loop. Ready queue size: %d, Running queue size: %d\n\n", readyQueue->size, runningQueue->size);
    
    // Close log file and clean up memory
    fclose(logFile);
    destroy(readyQueue);
    destroy(runningQueue);
    destroy(completeQueue);
}

void MLFQProcessSimulator(){
    // Initializing the queues
    EntryList* queue1 = initialize();
    EntryList* queue2 = initialize();
    EntryList* queue3 = initialize();
    EntryList* queue4 = initialize();
    EntryList* queue5 = initialize();
    
    EntryList* readyQueue = NULL;
    EntryList* runningQueue = initialize();
    EntryList* completeQueue = initialize();
    
    // Initially set the time variables to 0 (start of global time) 
    float cputime = globalTime;
    float arrivaltime = globalTime;
    float turnaroundtime = globalTime;
    float responsetime = globalTime;
    float cputimeSlice = globalTime;
    
    int firstexecflag = 0; // Flag to indicate if a process is executing for the first time
    float quantumCounter = 0.0f;
    
    int indicator = 1;
    

    // Open log files to record simulated results
    FILE* logFile = fopen("../log/MLFQLog.txt", "w");
    if(logFile == NULL){
        printf("Error opening log file.\n");
        return;
    }
    
    printf("MLFQ Simulation started.\n");
    
    do{

        // Read from files and add processes to readyQueue
        int niceness;
        float proctime;
        char* path = "../newProc/";
        while(readFromFile(path, &proctime, &niceness) != FAIL_READ_FILE){
            
            int originalPriority = setPriority(niceness);
            int currentPriority = originalPriority;
            
        
            // Create a new process entry and add it to readyQueue
            Entry* newEntry = createEntry(++currentPID, READY, niceness, cputime, proctime, arrivaltime, turnaroundtime, responsetime, cputimeSlice, firstexecflag, indicator, originalPriority, currentPriority);
            newEntry->arrivaltime += globalTime; // Increment arrivaltime for the next process
            
            if (newEntry->originalPriority == 5){
                pushEntry(queue5, newEntry);
            }
            
            else if (newEntry->originalPriority == 4){
                pushEntry(queue4, newEntry);
            }
            
            else if (newEntry->originalPriority == 3){
                pushEntry(queue3, newEntry);
            }
            
            else if (newEntry->originalPriority == 2){
                pushEntry(queue2, newEntry);
            }
            
            else if (newEntry->originalPriority == 1){
                pushEntry(queue1, newEntry);
            }
            globalTime += TIME_DT;
            
            printf("Processing pid = %d\n", newEntry->pid);
 
            
        }
        
        
        // Process all ready processes
        if (runningQueue->size == 0) {
        
            EntryList* queues[] = {queue5, queue4, queue3, queue2, queue1};

            // Fetch the process from the highest priority non-empty queue
            Entry* readyProcess = NULL;
        
            for (int i=0; i< 5; i++){
                if (queues[i]->size != 0){
                    readyProcess = popEntry(queues[i]);
                    printf("The PID is %d\n", readyProcess->pid);
                
                    // Calculate response time for the process if it's their first time executing
                    if (readyProcess->firstexecFlag == 0) {
                        readyProcess->responsetime = globalTime - readyProcess->arrivaltime;
                        readyProcess->firstexecFlag += 1; // Set the flag to 1 to indicate first execution
                    }
       
                    readyProcess->status = RUNNING; // Move to running state
                    pushEntry(runningQueue, readyProcess);
                    break;
                }
            }
   
       

        
        }


        // Increment the global time for each cycle
        globalTime += TIME_DT; 

        // Update running processes
        Entry* runningProcess = runningQueue->head;
        
        if(runningProcess != NULL){
	    runningProcess->cputime += TIME_DT;
	    quantumCounter += TIME_DT;
	    runningProcess->cputimeSlice += TIME_DT;
	    
	    printf("cputime is %.2f\n", runningProcess->cputime);
	    printf("Time quantum is %.2f\n", quantumCounter);
		
	    // Check if the process has completed execution
	    if(runningProcess->cputime >= runningProcess->proctime){
		
		// If the process is complete, move it to the completeQueue
		runningProcess->cputime = runningProcess->proctime; // Completed process execution
		
		runningProcess->turnaroundtime = globalTime - runningProcess->arrivaltime; // Calculate turnaround time

                //printf("PID = %d has completed at %.2f\n", runningProcess->pid, runningProcess->cputime);
                	
		resetCputimeSlice(runningProcess); // Reset cputimeSlice to 0 in order to track the time in the running queue before popped
		
	        Entry* completeProcess = popEntry(runningQueue);
		completeProcess->status = COMPLETE;
		quantumCounter = 0;
		pushEntry(completeQueue, completeProcess);
		
		// Log the state of queues to the file
		logState(4, logFile, readyQueue, queue1, queue2, queue3, queue4, queue5, runningQueue, completeQueue);
		
	      // If quantum time elapsed and process is not complete, preempt the process
            } else if(quantumCounter > TIME_JIFFY && runningProcess->cputime < runningProcess->proctime){
            
                //printf("Quantum counter is %.2f\n", quantumCounter);
        	// Log the state of queues to the file
                logState(4, logFile, readyQueue, queue1, queue2, queue3, queue4, queue5, runningQueue, completeQueue);      	 
                resetCputimeSlice(runningProcess); // Reset cputimeSlice to 0 in order to track the time in the running queue
                //resetCputimeSlice(runningProcess); // Reset cputimeSlice to 0 in order to track the time in the running queue before popped

                Entry* preemptedProcess = popEntry(runningQueue);
                printf("Process = %d\n", preemptedProcess->pid);
                printf("preemtedProcess PIDF = %d\n", preemptedProcess->currentPriority);
                
                if (runningProcess->currentPriority == 5){
            	    
            	    pushEntry(queue4, preemptedProcess);

            	    preemptedProcess->status = READY;  // Move the process back to the readyQueue
            	    preemptedProcess->currentPriority = 4;
            	    quantumCounter = 0;
            	}
            	
            	else if (runningProcess->currentPriority == 4){
            	    
            	    pushEntry(queue3, preemptedProcess);
            	    preemptedProcess->status = READY;  // Move the process back to the readyQueue
            	    preemptedProcess->currentPriority = 3;
            	    quantumCounter = 0;
            	}
            	
            	else if (runningProcess->currentPriority == 3){
            	    
            	    pushEntry(queue2, preemptedProcess);
            	    preemptedProcess->status = READY;  // Move the process back to the readyQueue
            	    preemptedProcess->currentPriority = 2;
            	    quantumCounter = 0;
            	}
            	
            	else if (runningProcess->currentPriority == 2){
            	    
            	    pushEntry(queue1, preemptedProcess);

            	    preemptedProcess->status = READY;  // Move the process back to the readyQueue
            	    preemptedProcess->currentPriority = 1;
            	    quantumCounter = 0;
            	}
            	
            	else if (runningProcess->currentPriority == 1){
            	    
            	    pushEntry(queue1, preemptedProcess);
            	    preemptedProcess->status = READY;  // Move the process back to the readyQueue
            	    quantumCounter = 0;
            	}
                
               	
		preemptedProcess->indicator++;

            }
	    else{
		// Log the state of queues to the file
                logState(4, logFile, readyQueue, queue1, queue2, queue3, queue4, queue5, runningQueue, completeQueue);
	    }
        
	}	
        
        
        
        

        
            
            if (globalTime >= TIME_RESET){
                resetProcessesToOriginalQueues(runningQueue, queue1, queue2, queue3, queue4, queue5);
                
                logState(4, logFile, readyQueue, queue1, queue2, queue3, queue4, queue5, runningQueue, completeQueue);
                printf("Time reset has reached! Resetting all processes back to their original priority level queues\n");
                break;
            }
            
        
           

    }while(!(queue1->size == 0 && queue2->size == 0 && queue3->size == 0 && queue4->size == 0 && queue5->size == 0 && runningQueue->size == 0));
    
  

    printf("\nMLFQ Simulation completed.\n");
    //printf("End of simulation loop. Ready queue size: %d, Running queue size: %d\n\n", readyQueue->size, runningQueue->size);
    
    // Close log files and clean up memory
    fclose(logFile);
    destroy(queue1);
    destroy(queue2);
    destroy(queue3);
    destroy(queue4);
    destroy(queue5);
    destroy(runningQueue);
    destroy(completeQueue);
    
}


void runSimulation(int algorithm){
    if(algorithm == ALGOR_FIFO){
        FIFOProcessSimulator();
    }
    
    else if(algorithm == ALGOR_SJF){
        SJFProcessSimulator();
    }
    
    else if(algorithm == ALGOR_RR){
        roundRobinProcessSimulator();
    }
    
    else if(algorithm == ALGOR_MLFQ){
        MLFQProcessSimulator();
    }
    
    moveFilesFromBackup();
}


/**
  *@brief Run the program
  *@param none
  *@return 0
  */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <algorithm>\n", argv[0]);
        return 1;
    }

    int algorithm = atoi(argv[1]);
    runSimulation(algorithm);

    return 0;
}
