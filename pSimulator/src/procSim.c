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
#define TIME_DT 1.0


/**
  *@file procSim.c
  *@brief This file sets up a process simulator
  *@author Salah Mohamed & Vy Tran
  *@date 04022024
  *@version 9
 */

int currentPID = 0;
double globalTime = 0.0;
int fileNumber = 1;
double quantumCounter = 0.0;
//double deltaCputime = 0.1;

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
  *@brief Reading the input files to extract proctime and niceness values
  *@param path, proctime, niceness
  *@return 1 or 0
  */

int readFromFile(const char *path, double *proctime, int *niceness){
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
                   		fscanf(fp, "%lf %d", proctime, niceness);
                    		printf("Read from file %s: Proctime: %.2f, Niceness: %d\n", entry->d_name, *proctime, *niceness);
                    		fclose(fp);
				// Deleting the file after reading
				if(remove(filePath) == 0){
                	        	printf("Removed file: %s\n", filePath);
			    	} 
			    	else{
                        		perror("Error removing file");
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
        fprintf(file, "%.2f, %d, %d, %d, %.2f, %.2f\n",
                globalTime, current->pid, current->status, current->niceness,
                current->cputime, current->proctime);
        current = current->next;
    }

}

/**
  *@brief Log the state of queues to a file
  *@param logFile, readyQueue, runningQueue
  *@return none
  */
void logState(FILE* logFile, EntryList* readyQueue, EntryList* runningQueue){
    printAllEntriesToFile(logFile, readyQueue);
    fprintf(logFile, "\n");

    printAllEntriesToFile(logFile, runningQueue);
    fprintf(logFile, "\n\n");
}

void printProcessTimesToFile(FILE* file, EntryList* completeQueue){
    Entry* current = completeQueue->head;
    
    while(current != NULL){
        fprintf(file, "%d, %.2lf, %.2lf, %.2lf\n", current->pid, current->turnaroundtime, current->responsetime, current->arrivaltime);
        current = current->next;
    }
}

/**
  *@brief Write the final times when the whole process is completed
  *@param logFile, completeQueue
  *@return none
  */
void writeCompleteLog(FILE* logFile, EntryList* completeQueue){
    printAllEntriesToFile(logFile, completeQueue);
    fprintf(logFile, "\n");
    printProcessTimesToFile(logFile, completeQueue);
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
    
    // Initially set the time variables to 0 (start of global time) 
    double cputime = globalTime;
    double arrivaltime = globalTime;
    double turnaroundtime = globalTime;
    double responsetime = globalTime;
    
    int firstexecflag = 0; // Flag to indicate if a process is executing for the first time

    // Open log files to record simulated results
    FILE* logFile = fopen("../log/FIFOLog.txt", "w");
    if(logFile == NULL){
        printf("Error opening log file.\n");
        return;
    }

    FILE* CompleteLogFile = fopen("../log/FIFOCompleteLog.txt", "w");
    if(CompleteLogFile == NULL){
        printf("Error opening complete log file.\n");
        return;
    }

    printf("Simulation started.\n");

    do{

        // Read from files and add processes to readyQueue
        int niceness;
        double proctime;
        char* path = "../newProc/";
        if(readFromFile(path, &proctime, &niceness)!= 0){
        
            // Create a new process entry and add it to readyQueue
            Entry* newEntry = createEntry(++currentPID, READY, niceness, cputime, proctime, arrivaltime, turnaroundtime, responsetime, firstexecflag);
            pushEntry(readyQueue, newEntry);
            arrivaltime = globalTime; // Increment arrivaltime for the next process
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
        
        // Increment the global time for each cycle
        globalTime += TIME_DT; 

        // Update running processes
        Entry* runningProcess = runningQueue->head;

        if(runningProcess != NULL){
	    runningProcess->cputime += TIME_DT;
		
	    // Check if the process has completed execution
	    if(runningProcess->cputime >= runningProcess->proctime){
	    
	        // If the process is complete, move it to the completeQueue and calculate turnaroundtime
	        Entry* completeProcess = popEntry(runningQueue);
		completeProcess->status = COMPLETE;
		completeProcess->turnaroundtime = globalTime - completeProcess->arrivaltime; // Calculate turnaround time
		pushEntry(completeQueue, completeProcess);
            }
        }

        // Log the state of queues to the file
        logState(logFile, readyQueue, runningQueue);

    }while(!(readyQueue->size == 0 && runningQueue->size == 0));
    
    // Write a separate log file for the complete queue
    writeCompleteLog(CompleteLogFile, completeQueue);    

    printf("\nFIFO Simulation completed.\n");
    printf("End of simulation loop. Ready queue size: %d, Running queue size: %d\n\n", readyQueue->size, runningQueue->size);
    
    // Close log files and clean up memory
    fclose(logFile);
    fclose(CompleteLogFile);

    destroy(readyQueue);
    destroy(runningQueue);
    destroy(completeQueue);
}

void roundRobinProcessSimulator(){

    // Initializing the queues
    EntryList* readyQueue    = initialize();
    EntryList* runningQueue  = initialize();
    EntryList* completeQueue = initialize();
    
    // Initially set the time variables to 0 (start of global time) 
    double cputime = globalTime;
    double arrivaltime = globalTime;
    double turnaroundtime = globalTime;
    double responsetime = globalTime;
    
    int firstexecflag = 0; // Flag to indicate if a process is executing for the first time
    int quantumCounter = 0; // Counter for quantum comparison

    // Open log files to record simulated results
    FILE* logFile = fopen("../log/roundrobinLog.txt", "w");
    if(logFile == NULL){
        printf("Error opening log file.\n");
        return;
    }

    FILE* CompleteLogFile = fopen("../log/roundrobinCompleteLog.txt", "w");
    if(CompleteLogFile == NULL){
        printf("Error opening complete log file.\n");
        return;
    }

    printf("Simulation started.\n");

    do{

        // Read from files and add processes to readyQueue
        int niceness;
        double proctime;
        char* path = "../newProc/";
        
        if(readFromFile(path, &proctime, &niceness)!= 0){
        
            // Create a new process entry and add it to readyQueue
            Entry* newEntry = createEntry(++currentPID, READY, niceness, cputime, proctime, arrivaltime, turnaroundtime, responsetime, firstexecflag);
            pushEntry(readyQueue, newEntry);
            arrivaltime = globalTime; // Increment arrivaltime for the next process
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
		
	    // Check if the process has completed execution
	    if(runningProcess->cputime >= runningProcess->proctime){
		
		// If the process is complete, move it to the completeQueue and calculate turnaroundtime
	        Entry* completeProcess = popEntry(runningQueue);
		completeProcess->status = COMPLETE;
		completeProcess->turnaroundtime = globalTime - completeProcess->arrivaltime; // Calculate turnaround time
		pushEntry(completeQueue, completeProcess);
		
	      // If quantum time elapsed and process is not complete, preempt the process
            } else if(quantumCounter == TIME_JIFFY && runningProcess->cputime < runningProcess->proctime){
            	Entry* preemptedProcess = popEntry(runningQueue);
            	    
            	preemptedProcess->status = READY;  // Move the process back to the readyQueue
            	pushEntry(readyQueue, preemptedProcess);
            	quantumCounter = 0; // Reset the quantum counter
            }
        }

        // Log the state of queues to the file
        logState(logFile, readyQueue, runningQueue);  
	

    }while(!(readyQueue->size == 0 && runningQueue->size == 0));
    
    // Write a separate log file for the complete queue
    writeCompleteLog(CompleteLogFile, completeQueue);    

    printf("\nRound Robin Simulation completed.\n");
    printf("End of simulation loop. Ready queue size: %d, Running queue size: %d\n\n", readyQueue->size, runningQueue->size);
    
    // Close log files and clean up memory
    fclose(logFile);
    fclose(CompleteLogFile);
    destroy(readyQueue);
    destroy(runningQueue);
    destroy(completeQueue);
}


void SJFProcessSimulator(){

    // Initializing the queues
    EntryList* readyQueue    = initialize();
    EntryList* runningQueue  = initialize();
    EntryList* completeQueue = initialize();
    
    // Initially set the time variables to 0 (start of global time) 
    double cputime = globalTime;
    double arrivaltime = globalTime;
    double turnaroundtime = globalTime;
    double responsetime = globalTime;
    
    int firstexecflag = 0; // Flag to indicate if a process is executing for the first time

    // Open log files to record simulated results
    FILE* logFile = fopen("../log/SJFLog.txt", "w");
    if(logFile == NULL){
        printf("Error opening log file.\n");
        return;
    }

    FILE* CompleteLogFile = fopen("../log/SJFCompleteLog.txt", "w");
    if(CompleteLogFile == NULL){
        printf("Error opening complete log file.\n");
        return;
    }

    printf("Simulation started.\n");

    do{

        // Read from files and add processes to readyQueue
        int niceness;
        double proctime;
        char* path = "../newProc/";
        if(readFromFile(path, &proctime, &niceness)!= 0){
        
            // Create a new process entry and add it to readyQueue
            Entry* newEntry = createEntry(++currentPID, READY, niceness, cputime, proctime, arrivaltime, turnaroundtime, responsetime, firstexecflag);
            pushEntry(readyQueue, newEntry);
            arrivaltime = globalTime; // Increment arrivaltime for the next process
        }
        
        selectionSortByProctime(readyQueue); // Sort the ready queue in ascending order for SJF algorithm
         	
   
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
        
        // Increment the global time for each cycle
        globalTime += TIME_DT; 

        // Update running processes
        Entry* runningProcess = runningQueue->head;

        if(runningProcess != NULL){
	    runningProcess->cputime += TIME_DT;
		
	    // Check if the process has completed execution
	    if(runningProcess->cputime >= runningProcess->proctime){
	    
	        // If the process is complete, move it to the completeQueue and calculate turnaroundtime
	        Entry* completeProcess = popEntry(runningQueue);
		completeProcess->status = COMPLETE;
		completeProcess->turnaroundtime = globalTime - completeProcess->arrivaltime; // Calculate turnaround time
		pushEntry(completeQueue, completeProcess);
            }
        }

        // Log the state of queues to the file
        logState(logFile, readyQueue, runningQueue);

    }while(!(readyQueue->size == 0 && runningQueue->size == 0));
    
    // Write a separate log file for the complete queue
    writeCompleteLog(CompleteLogFile, completeQueue);    

    printf("\nSJF Simulation completed.\n");
    printf("End of simulation loop. Ready queue size: %d, Running queue size: %d\n\n", readyQueue->size, runningQueue->size);
    
    // Close log files and clean up memory
    fclose(logFile);
    fclose(CompleteLogFile);
    destroy(readyQueue);
    destroy(runningQueue);
    destroy(completeQueue);
}


/**
  *@brief Run the program
  *@param none
  *@return 0
  */
int main(void){

    int choice = ALGOR_FIFO;
    
    if(choice == ALGOR_RR){
        roundRobinProcessSimulator();
    } else if (choice == ALGOR_FIFO){
        FIFOProcessSimulator();
    } else if(choice == ALGOR_SJF){
        SJFProcessSimulator();
    }
    return(0);
}
