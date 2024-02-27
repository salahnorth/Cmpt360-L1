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

#define TIME_JIFFY 2
#define TIME_DT 0.1

#define SOURCE_DIR "../newProc/"
#define BACKUP_DIR "../backup_newProc/"


/**
  *@file procSim.c
  *@brief This file sets up a process simulator
  *@author Salah Mohamed & Vy Tran
  *@date 26022024
  *@version 10
 */

int currentPID = 0;
float globalTime = 0.0f;
int fileNumber = 1;
float quantumCounter = 0.0f;


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

/**
  *@brief Print all the entries to a file
  *@param file, EntryList
  *@return none
  */
  
void printAllEntriesToFile(FILE* file, EntryList* entryList){

    Entry* current = entryList->head;

    while(current != NULL){
        fprintf(file, "%.2f, %d, %d, %d, %.2f, %.2f, %.2f, %.2f, %.2f, %d\n",
                globalTime, current->pid, current->status, current->niceness,
                current->cputime, current->cputimeSlice, current->proctime,
		current->turnaroundtime, current->responsetime,
		current->indicator);
        current = current->next;
    }

}

/**
  *@brief Log the state of queues to a file
  *@param logFile, readyQueue, runningQueue
  *@return none
  */
void logState(FILE* logFile, EntryList* readyQueue, EntryList* runningQueue, EntryList* completeQueue){

    printAllEntriesToFile(logFile, readyQueue);
    fprintf(logFile, "\n");
    printAllEntriesToFile(logFile, runningQueue);
    fprintf(logFile, "\n\n");
    printAllEntriesToFile(logFile, completeQueue);
    fprintf(logFile, "\n\n\n");
}

/**
  *@brief Simulate the process executiong cycle for FIFO
  *@param none
  *@return none
  */
  
void FIFOProcessSimulator(){
    // Initializing the queues
    EntryList* readyQueue    = initialize();
    EntryList* runningQueue  = initialize();
    EntryList* completeQueue = initialize();
    
    // Initially set the time variables to 0 (start of global time) 
    float cputime = globalTime;
    float arrivaltime = globalTime;
    float turnaroundtime = globalTime;
    float responsetime = globalTime;
    float cputimeSlice = globalTime;
    int indicator = 0;	// Process is changed inside the running queue
    
    int firstexecflag = 0; // Flag to indicate if a process is executing for the first time

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
        if(readFromFile(path, &proctime, &niceness)!= 0){
        
            /// Create a new process entry and add it to readyQueue
            Entry* newEntry = createEntry(++currentPID, READY, niceness,
			    cputime, proctime, arrivaltime, turnaroundtime,
			    responsetime, cputimeSlice, firstexecflag, indicator);
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
        logState(logFile, readyQueue, runningQueue, completeQueue);
        
        

    }while(!(readyQueue->size == 0 && runningQueue->size == 0));  

    printf("\nFIFO Simulation completed.\n");
    printf("End of simulation loop. Ready queue size: %d, Running queue size: %d\n\n", readyQueue->size, runningQueue->size);
    
    // Close log file and clean up memory
    fclose(logFile);
    destroy(readyQueue);
    destroy(runningQueue);
    destroy(completeQueue);
}

/**
  *@brief Simulate the process executiong cycle for FIFO
  *@param none
  *@return none
  */
void SJFProcessSimulator(){

    // Initializing the queues
    EntryList* readyQueue    = initialize();
    EntryList* runningQueue  = initialize();
    EntryList* completeQueue = initialize();
    
    // Initially set the time variables to 0 (start of global time) 
    float cputime = globalTime;
    float arrivaltime = globalTime;
    float turnaroundtime = globalTime;
    float responsetime = globalTime;
    float cputimeSlice = globalTime;
    int indicator = 0;

    int firstexecflag = 0; // Flag to indicate if a process is executing for the first time

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
        while(readFromFile(path, &proctime, &niceness)!= 0){
        
            /// Create a new process entry and add it to readyQueue
            Entry* newEntry = createEntry(++currentPID, READY, niceness,
			    cputime, proctime, arrivaltime, turnaroundtime,
			    responsetime, cputimeSlice, firstexecflag, indicator);
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
        logState(logFile, readyQueue, runningQueue, completeQueue);


    }while(!(readyQueue->size == 0 && runningQueue->size == 0));

    printf("\nSJF Simulation completed.\n");
    printf("End of simulation loop. Ready queue size: %d, Running queue size: %d\n\n", readyQueue->size, runningQueue->size);
    
    // Close log file and clean up memory
    fclose(logFile);
    destroy(readyQueue);
    destroy(runningQueue);
    destroy(completeQueue);
}

/**
  *@brief Simulate the process executiong cycle for RR
  *@param none
  *@return none
  */
void roundRobinProcessSimulator(){

    // Initializing the queues
    EntryList* readyQueue    = initialize();
    EntryList* runningQueue  = initialize();
    EntryList* completeQueue = initialize();
    
    // Initially set the time variables to 0 (start of global time) 
    float cputime = globalTime;
    float arrivaltime = globalTime;
    float turnaroundtime = globalTime;
    float responsetime = globalTime;
    float cputimeSlice = globalTime;

    int indicator = 1;
    
    int firstexecflag = 0; // Flag to indicate if a process is executing for the first time
    float quantumCounter = 0.0f; // Counter for quantum comparison

    // Open log files to record simulated results
    FILE* logFile = fopen("../log/roundrobinLog.txt", "w");
    if(logFile == NULL){
        printf("Error opening log file.\n");
        return;
    }

    printf("Round Robin Simulation started.\n");

    do{

        // Read from files and add processes to readyQueue
        int niceness;
        float proctime;
        char* path = "../newProc/";
        
        if(readFromFile(path, &proctime, &niceness)!= 0){
        
            // Create a new process entry and add it to readyQueue
            Entry* newEntry = createEntry(++currentPID, READY, niceness,
			    cputime, proctime, arrivaltime, turnaroundtime,
			    responsetime, cputimeSlice, firstexecflag, indicator);
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
	 	logState(logFile, readyQueue, runningQueue, completeQueue);	
		resetCputimeSlice(runningProcess); // Reset cputimeSlice to 0 in order to track the time in the running queue before popped
		
	        Entry* completeProcess = popEntry(runningQueue);
		completeProcess->status = COMPLETE;
		
		pushEntry(completeQueue, completeProcess);
		
	      // If quantum time elapsed and process is not complete, preempt the process
            } else if(quantumCounter > TIME_JIFFY && runningProcess->cputime < runningProcess->proctime){
        	logState(logFile, readyQueue, runningQueue, completeQueue);       	 
                
                resetCputimeSlice(runningProcess); // Reset cputimeSlice to 0 in order to track the time in the running queue before popped
               	Entry* preemptedProcess = popEntry(runningQueue);  
            	preemptedProcess->status = READY;  // Move the process back to the readyQueue
          
            	pushEntry(readyQueue, preemptedProcess);
            	
            	quantumCounter = 0; // Reset the quantum counter
		preemptedProcess->indicator++;

            }
	    else{
		logState(logFile, readyQueue, runningQueue, completeQueue);
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

/**
  *@brief Run the program based on algorithm
  *@param algorithm. Integer identifier for chosen algorithm
  *@return 0
  */
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
    moveFilesFromBackup();
}


/**
  *@brief Run program with specified algorithm
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
