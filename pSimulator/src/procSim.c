#include <stdio.h>
#include <stdlib.h>
#include "dStruct.h"

#define READY 1
#define RUNNING 2
#define COMPLETE 3

int currentPID = 0;
int globalTime = 0;
int fileNumber = 1;
double deltaCputime = 0.1;


int readFromFile(int *niceness, double *proctime, int fileNumber) {
    FILE *fp1;

    char fileName[100];

    snprintf(fileName, sizeof(fileName), "../newProc/file%d.txt", fileNumber);

    fp1 = fopen(fileName, "r");

    if (fp1 == NULL) {
        printf("File %s doesn't exist.\n", fileName);
		return 0;
    } else {
        fscanf(fp1, "%d %lf", niceness, proctime);
        printf("Read from file %s: Niceness: %d, Proctime: %.2f\n", fileName, *niceness, *proctime);
        fclose(fp1);
		return 1;
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
}

// Function to log the state of queues to a file
void logState(FILE* logFile, int time, EntryList* readyQueue, EntryList* runningQueue) {
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
        printf("Cycle %d\n", globalTime);

        // Read from files and add processes to readyQueue
        int niceness;
        double proctime;
        if (readFromFile(&niceness, &proctime, fileNumber)!= 0) {
            Entry* newEntry = createEntry(++currentPID, READY, niceness, 0.0, proctime);
            pushEntry(readyQueue, newEntry);
            fileNumber++;
        }

		printf("After reading files. Ready queue size: %d, Running queue size: %d\n", readyQueue->size, runningQueue->size);

        // Process all ready processes
        if (runningQueue->size == 0) {
            Entry* readyProcess = popEntry(readyQueue);
            readyProcess->status = RUNNING;  // Move to running state
            pushEntry(runningQueue, readyProcess);
        }

		printf("After processing ready queue. Ready queue size: %d, Running queue size: %d\n", readyQueue->size, runningQueue->size);
        // Update running processes
        Entry* runningProcess = popEntry(runningQueue);
        if (runningProcess != NULL) {
            runningProcess->cputime += deltaCputime;  // Increment CPU time

            if (runningProcess->cputime >= runningProcess->proctime) {
                // Process is complete
                runningProcess->status = COMPLETE;  // Move to complete state
                pushEntry(completeQueue, runningProcess);
            } else {
                // Process still running, push back to running queue
                pushEntry(runningQueue, runningProcess);
            }
        }

        // Log the state of queues to the file
        logState(logFile, globalTime, readyQueue, runningQueue);
		printf("Logged in\n");

        // Increment the global time for each cycle
        globalTime++;
    } while (!(readyQueue->size == 0 && runningQueue->size == 0));


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
