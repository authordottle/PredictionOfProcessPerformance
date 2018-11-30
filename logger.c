/***CSE 2431 Project***/
/***Jiaqian Huang   ***/
/***Andrew Maloney  ***/
/***Alec Wilson     ***/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#define ITERATION 10
#define ALPHA 0.5

volatile int alarmFlag = 0;

long int nextBurstTime(long int, double, long int);
int oneMoreTime();
void getFilePath(char*);
int getTicks(char*, long*, long*);

void alarmHandler(int sig){
	alarmFlag = 1;
}

int main() {
	//file pointer for log file and read file
	FILE *logFile;

	//string to hold full file path
	char filePath[100];

	//longs to hold utimes and stimes (both are clock ticks)
	long utime, stime;
	long lutime, lstime;
	utime=stime=lutime=lstime=0;

	//longs to hold delta utime stime
	long dutime, dstime;

	//long to hold predicted next clock ticks
	long taunu=10, tauns=10;
	long ltaunu=10, ltauns=10;

	//counter for loop
	int loopCounter = 0;//loop variable
	int dowhileCount = 1;//count iterations

	//open file to log data at user's request
	printf("Would you like to log this data? (Y for yes): ");
	char response = getchar();
	int logging=0;
	if(response == 'Y' || response == 'y'){
		logFile = fopen("log.txt", "w");
		if(logFile == NULL){
			printf("Could not open log file\n");
			return 0;
		}
		logging=1;
	}

	//loop if user wants to continue
	do{
		if(logging){
			fprintf(logFile, "Iteration%d:\n", dowhileCount);
		}

		//get maps file path
		getFilePath(filePath);
		if(logging){
			fprintf(logFile, "%s\n", filePath);
		}

		//signal so that alarm can be used
		signal(SIGALRM, alarmHandler);

		//initialize alarmFlag to 0
		alarmFlag = 0;
		alarm(1);
		loopCounter = 0;

		//Iterate through process measurement ITERATION times
		while(loopCounter<ITERATION){
			//if alarm goes off
			if(alarmFlag){
				//get user ticks and system ticks
				if(!getTicks(filePath, &utime, &stime))
					return 0;

				//write ticks
				if(logging){
					fprintf(logFile, "%lu %lu\n", utime, stime);
				}
				printf("\nUser Ticks: %lu\tSystem Ticks: %lu\n", utime, stime);

				//Print difference if applicable
				if(loopCounter>0){
					dutime = utime-lutime;
					dstime = stime-lstime;
					taunu = nextBurstTime(dutime, ALPHA, ltaunu);
					tauns = nextBurstTime(dstime, ALPHA, ltauns);
					printf("User delta: %lu\tSystem delta: %lu\n", dutime, dstime);
					printf("Next User: %lu\tNext System: %lu\n", taunu, tauns);
				}
				//update last tick count
				lutime=utime;
				lstime=stime;
				ltaunu=taunu;
				ltauns=tauns;

				//increment loopCounter
				loopCounter++;

				//reset alarm
				alarmFlag = 0;
				alarm(1);
			}
		} 
		dowhileCount++;
		if(logging){
			fprintf(logFile, "\n");
		}
	}while(oneMoreTime());

	//close log file if necessary
	if(logging){
		fclose(logFile);
	}
}


//Calculates next process burst time using exponential averaging algorithm
//Returns next process burst time (CPU ticks)
long int nextBurstTime(long int tn, double alpha, long int taun){
	long int nextBurst; 
	nextBurst = alpha * tn + (1 - alpha) * taun;
	return nextBurst;
}

//Polls to the user whether to run again or not
//Returns: 1 if user wants to run again
//         0 otherwise
int oneMoreTime(){
	char cont;

	printf("Would you like to run again? (Y to run again): ");
	getchar();
	cont = getchar();

	return(cont == 'Y' || cont == 'y');
}

//Gets file path to read process information based on user input
//Replaces: filePath
void getFilePath(char *filePath){
	int neededPID=1;

	//get pid from user
	printf("Please enter PID of process to measure: ");
	scanf("%d", &neededPID);

	//create string for file path
	sprintf(filePath, "/proc/%d/stat", neededPID);
}

//Gets user and system ticks
//Replaces: utime, stime
//Returns: 1 if successful
//         0 otherwise
int getTicks(char* filePath, long* utime, long* stime){
	//File pointer for read
	FILE *readFile;

	//open /proc/<neededPID> folder
	readFile = fopen(filePath, "r");
	if(readFile == NULL){
		printf("Could not open %s\n", filePath);
		return 0;
	}

	//read utume and stime from readFile aka the scariest fscanf in the west
	fscanf(readFile,"%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", utime, stime);

	//offset to beginning of file
	//rewind(readFile);
	fclose(readFile);

	return 1;
}
