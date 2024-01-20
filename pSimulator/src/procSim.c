#include<stdio.h>
#include"dStruct.h"

int main(void){
	int niceness;
	double proctime;

	FILE *fp1;
	
	fp1 = fopen("/home/chipcaptain/360lab/lab2/pSimulator/newProc/file1.txt", "r");
	if(fp1 == NULL){
		printf("File doesn't exist.");
	}
	else{
		fscanf(fp1,"%d %lf",&niceness, &proctime);
		printf("Niceness & proctime from file: %d %.1f\n", niceness,
				proctime);
		fclose(fp1);
	}
	return(0);
}

