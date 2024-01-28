#include <stdio.h>
#include <stdlib.h>
#include "dStruct.h"

#define READY 1
#define RUNNING 2
#define COMPLETE 3

int currentPID = 0;
int globalTime = 0;
int fileNumber = 1;
int finProcFileNumber = 1;
double deltaCputime = 1.0;


int readFromFile(double *proctime, int *niceness, int fileNumber, int finProcFileNumber) {
    FILE *fp1;

    char fileName[100];

    // Try to read from newProc folder first
    snprintf(fileName, sizeof(fileName), "../newProc/file%d.txt", fileNumber);
    fp1 = fopen(fileName, "r");

    if (fp1 == NULL) {
        // If file not found in newProc, try finProc
        snprintf(fileName, sizeof(fileName), "../finProc/file%d.txt", finProcFileNumber);
        fp1 = fopen(fileName, "r");

        if (fp1 == NULL) {
            printf("No more files to process.\n");
            return 0;
        } else {
            finProcFileNumber++;
        }
    } else {
        fileNumber++;
    }

    // Rest of the function remains the same
    fscanf(fp1, "%lf %d", proctime, niceness);
    printf("Read from file %s: Proctime: %.2f, Niceness: %d\n", fileName, *proctime, *niceness);
    fclose(fp1);
    return 1;
}


void moveProcessedFile(int fileNumber) {
    char sourceFileName[100];
    char destinationFileName[100];

    snprintf(sourceFileName, sizeof(sourceFileName), "../newProc/file%d.txt", fileNumber);
    snprintf(destinationFileName, sizeof(destinationFileName), "../finProc/file%d.txt", fileNumber);

    if (rename(sourceFileName, destinationFileName) == 0) {
        printf("Moved file %s to %s\n", sourceFileName, destinationFileName);
    } else {
        printf("Error moving file %s to %s\n", sourceFileName, destinationFileName);
    }
}


// Function to print all entries to a file with the desired format
void printAllEntriesToFile(FILE* file, EntryList* entryList) {

    Entry* current = entryList->head;

    while (current != NULL) {
        fprintf(file, "%d, %d, %d, %d, %.2f, %.2f\n",
                globalTime, current->pid, current->status, current->niceness,
                current->cputime, current->proctime);
        current = current->next;
    }
    //printEntryByPid(entryList, 3);
}

// Function to log the state of queues to a file
void logState(FILE* logFile, EntryList* readyQueue, EntryList* runningQueue) {
    //fprintf(logFile, "Ready Queue:\n");
    printAllEntriesToFile(logFile, readyQueue);
    fprintf(logFile, "\n");

    //fprintf(logFile, "\nRunning Queue:\n");
    printAllEntriesToFile(logFile, runningQueue);

    fprintf(logFile, "\n\n");
}

void writeCompleteLog(FILE* logFile, EntryList* completeQueue) {
    printAllEntriesToFile(logFile, completeQueue);
}

// Function to simulate the process execution cycle
void processSimulator() {
    EntryList* readyQueue = initialize();
    EntryList* runningQueue = initialize();
    EntryList* completeQueue = initialize();



    FILE* logFile = fopen("../log/log-01-19-24.txt", "w");
    if (logFile == NULL) {
        printf("Error opening log file.\n");
        return;
    }

    FILE* CompleteLogFile = fopen("../log/CompleteLog-01-19-24.txt", "w");
    if (CompleteLogFile == NULL) {
        printf("Error opening complete log file.\n");
        return;
    }

    printf("Simulation started.\n");

    do {

        //int flag = 0;

        // Increment the global time for each cycle
        globalTime ++;
        //fprintf(logFile, "===Cycle %d=== \n", globalTime);

        // Read from files and add processes to readyQueue
        int niceness;
        double proctime;
        while (readFromFile(&proctime, &niceness, fileNumber, finProcFileNumber)!= 0) {
            Entry* newEntry = createEntry(++currentPID, READY, niceness, 0.0, proctime);
            pushEntry(readyQueue, newEntry);
            moveProcessedFile(fileNumber); // Move the file to finProc folder
            fileNumber++;
            finProcFileNumber++;

        }

    
        // Process all ready processes
        if (runningQueue->size == 0) {
            Entry* readyProcess = popEntry(readyQueue);
            Entry* readProcess = createEntry(readyProcess->pid, readyProcess->status, readyProcess->niceness, readyProcess->cputime, readyProcess->proctime);
            
            if (readProcess != NULL){
                readProcess->status = RUNNING;  // Move to running state
                pushEntry(runningQueue, readProcess);
            }
            
        }

        // Update running processes

        Entry* runningProcess = runningQueue->head;

        if (runningProcess != NULL) {
            runningProcess->cputime += deltaCputime;  // Increment CPU time

            if (runningProcess->cputime >= runningProcess->proctime) {
                // Process is complete
                Entry* completeProcess = popEntry(runningQueue);
                completeProcess->status = COMPLETE;
                pushEntry(completeQueue, completeProcess);

                
            } 
        
                
        }

        // Log the state of queues to the file
        logState(logFile, readyQueue, runningQueue);
	    printf("Logged in\n");

    } while (!(readyQueue->size == 0 && runningQueue->size == 0));
    
    writeCompleteLog(CompleteLogFile, completeQueue); // Write a separate log file for the complete queue
    


    printf("Simulation completed.\n");

	printf("End of simulation loop. Ready queue size: %d, Running queue size: %d\n", readyQueue->size, runningQueue->size);

    fclose(logFile);
    destroy(readyQueue);
    destroy(runningQueue);
    destroy(completeQueue);
}


int main() {
    processSimulator();
    return 0;
}
