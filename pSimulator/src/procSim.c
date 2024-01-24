#include <stdio.h>
#include <stdlib.h>
#include "dStruct.h"

#define READY 1
#define RUNNING 2
#define COMPLETE 3

int currentPID = 0;
int globalTime = 0;
int fileNumber = 1;
double deltaCputime = 1.0;


int readFromFile(double *proctime, int *niceness, int fileNumber) {
    FILE *fp1;

    char fileName[100];

    snprintf(fileName, sizeof(fileName), "../newProc/file%d.txt", fileNumber);

    fp1 = fopen(fileName, "r");

    if (fp1 == NULL) {
        printf("File %s doesn't exist.\n", fileName);
		return 0;
    } else {
        fscanf(fp1, "%lf %d", proctime, niceness);
        printf("Read from file %s: Proctime: %.2f, Niceness: %d\n", fileName, *proctime, *niceness);
        fclose(fp1);
		return 1;
    }
}

// Function to print all entries to a file with the desired format
void printAllEntriesToFile(FILE* file, EntryList* entryList) {

    Entry* current = entryList->head;

    while (current != NULL) {
        fprintf(file, "//Time, PID, Status, Niceness, Cputime, Proctime\n%d, %d, %d, %d, %.2f, %.2f\n",
                globalTime, current->pid, current->status, current->niceness,
                current->cputime, current->proctime);
        current = current->next;
    }
}

// Function to log the state of queues to a file
void logState(FILE* logFile, EntryList* readyQueue, EntryList* runningQueue) {
    printAllEntriesToFile(logFile, readyQueue);
    printAllEntriesToFile(logFile, runningQueue);

    fprintf(logFile, "\n");
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

    printf("Simulation started.\n");

    do {

        //int flag = 0;

        logState(logFile, readyQueue, runningQueue);

        // Increment the global time for each cycle
        globalTime ++;
        printf("Cycle %d\n", globalTime);

        // Read from files and add processes to readyQueue
        int niceness;
        double proctime;
        if (readFromFile(&proctime, &niceness, fileNumber)!= 0) {
            Entry* newEntry = createEntry(++currentPID, READY, niceness, 0.0, proctime);
            pushEntry(readyQueue, newEntry);
            fileNumber++;
        }

		//printf("After reading files. Ready queue size: %d, Running queue size: %d\n", readyQueue->size, runningQueue->size);

    
        // Process all ready processes
        if (runningQueue->size == 0) {
            Entry* readyProcess = popEntry(readyQueue);
            readyProcess->status = RUNNING;  // Move to running state
            pushEntry(runningQueue, readyProcess);
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
        
        //logState(logFile, readyQueue, runningQueue);
        //printAllEntries(readyQueue);
	    printf("Logged in\n");

    } while (!(readyQueue->size == 0 && runningQueue->size == 0));
    

    printAllEntriesToFile(logFile, completeQueue);
    


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
