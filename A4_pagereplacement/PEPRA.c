#include <stdio.h>
#include <regex.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>


void *optimalPage(void *arg);
void *FIFO(void *arg);
void *leastRecent(void *arg);
void *mostRecent(void *arg);
void *leastFrequent(void *arg);
void *mostFrequent(void *arg);
void *randomSelect(void *arg);
void printReport(int number, int sequence[]);
void generateSequence(int sequence[]);
void initializeReportData();
void initializeBeladyReport();
void initializeBeladySummary();
void beladyReport();
void beladySummary();

int sequenceLength;
int reportData[9][7];
int beladyReportData[63][5];
int beladySummaryData[63][2];
int totalBeladyCount;
int trialCounter;

int main(int argc, char *argv[]){
	int numberTrials;
	char fileName[100];
	char input[1];
	int c,d,i;
	int sfd;
	pthread_t optimalThread;
	pthread_t fifoThread;
	pthread_t lruThread;
	pthread_t mruThread;
	pthread_t lfuThread;
	pthread_t mfuThread;
	pthread_t randThread;
	pthread_attr_t attr;
	
	if (argc == 2){
		sequenceLength = 0;
		strcpy(fileName, argv[1]);
		numberTrials = 1;
		FILE* file = fopen(fileName, "r");
		c = 0;
		fscanf (file, "%d", &c);
		while (!feof(file)){
			sequenceLength++;
			fscanf(file, "%d", &c);
		}
		fclose(file);
	}
	else if(argc == 3){
		sequenceLength = atoi(argv[1]);
		numberTrials = atoi(argv[2]);
		printf("Testing Belady's Anomaly using %d random sequences and Trying %d time\n", sequenceLength, numberTrials);
	}
	else{
		printf("Usage : \n");
		printf("PERPA <sequence-file> | <sequence-length> <trials-number> \n");
		exit(1);
	}
	
	int pageSequence[sequenceLength];
	
	if(argc == 2){
		FILE* file = fopen(fileName, "r");
		c = 0;
		d = 0;
		fscanf(file, "%d", &c);
		while(!feof(file)){
			pageSequence[d] = c;
			fscanf (file, "%d", &c);
			d++;
		}
		fclose(file);
	}
	printf("\n");
	srand(time(NULL) + getpid() *42);
	pthread_attr_init(&attr);
	
	initializeBeladySummary();
	totalBeladyCount = 0;
	
	for(i = 0; i < numberTrials; i++){
		trialCounter = i;
		
		//initializeReportData();
		//initializeBeladyReport();
		if(argc == 3){
			printf("Trial #%d\n", i+1);
			generateSequence(pageSequence);
		}
		else{
			printf("Using Sequence stored in File: %s\n", fileName);
		}
		pthread_create(&optimalThread, &attr, optimalPage, (void *) pageSequence);
		pthread_create(&fifoThread, &attr, FIFO, (void *) pageSequence);
		pthread_create(&lruThread, &attr, leastRecent, (void *) pageSequence);
		pthread_create(&mruThread, &attr, mostRecent, (void *) pageSequence);
		pthread_create(&lfuThread, &attr, leastFrequent, (void *) pageSequence);
		pthread_create(&mfuThread, &attr, mostFrequent, (void *) pageSequence);
		pthread_create(&randThread, &attr, randomSelect, (void *) pageSequence);
		sleep(2);
		printReport((i+1), pageSequence);
		beladyReport();
	}
	beladySummary();
	exit(0);
}

void *optimalPage(void* arg){
	int *sequence = (int *) arg;
	int memoryWidth;
	int numberReplacements;
	int pagePlaced;
	int pageExists;
	int i, j, k, l, m, n, o;
	int tempIndex;
	int tempDistance;
	int distanceFound;
	int memory[9][2];
	
	// Cycle through the different memory widths
	for(memoryWidth = 1; memoryWidth < 10; memoryWidth++){ // Cycle through the different memory widths
		
		// Clear the memory
		for(l = 0; l < memoryWidth; l++){
			memory[l][0] = 0; 				// Memory data reset
			memory[l][1] = 0; 				// Distance information reset
		}
		numberReplacements = 0;
		
		for(i = 0; i < sequenceLength; i++){ // Cycle through all of the pages in the sequence
			pagePlaced = 0;
			pageExists = 0;

			// Look for existing copies of the page
			if(pagePlaced == 0){
				for(k = 0; k < memoryWidth; k++){
					if(memory[k][0] == sequence[i]){
						pageExists = 1;
					}
				}
			}
			
			// Look for empty memory locations
			if(pageExists == 0){
				for(j = 0; j < memoryWidth; j++){
					if(memory[j][0] == 0 && pagePlaced == 0){
						memory[j][0] = sequence[i];
						pagePlaced = 1;
						numberReplacements++;
					}
				}
			}	
			
			// Find replacement place for page
			if(pagePlaced == 0 && pageExists == 0){
				
				// Find distances for all memory
				for(m = 0; m < memoryWidth; m++){ // Cycle through all memory locations
					distanceFound = 0; // Reset found boolean
					for(n = (i+1); n < sequenceLength; n++){ // Cycle through the remaining pages to be placed
						if(memory[m][0] == sequence[n] && distanceFound == 0){ // Look for matches in memory
							memory[m][1] = n - i;
							distanceFound = 1;
						}
					}
					if(distanceFound == 0){
						memory[m][1] = 999;
					}
				}
				
				// Find the greatest distance
				tempDistance = -999;
				tempIndex = 0;
				for(o = 0; o < memoryWidth; o++){
					if(memory[o][1] > tempDistance){
						tempDistance = memory[o][1];
						tempIndex = o;
					}
				}
				
				memory[tempIndex][0] = sequence[i]; // Place the page into memory
				 
				numberReplacements++;
			}
	
		} // End of Page Sequence Iteration
		reportData[memoryWidth-1][0] = numberReplacements;
	} 
}

void *FIFO(void* arg){
	
	int *sequence = (int *) arg;
	int memoryWidth = 0;
	int numberReplacements = 0;
	int index = 0;
	int pagePlaced = 0;
	int pageExists = 0;
	int i, j, k, l, m;
	int memory[9];
	
	for(memoryWidth = 1; memoryWidth < 10; memoryWidth++){
		index = 0;
		
		// Clear the memory
		for(l = 0; l < memoryWidth; l++){
			memory[l] = 0;
		}
		numberReplacements = 0;
		
		
		for(i = 0; i < sequenceLength; i++){
			pagePlaced = 0;
			pageExists = 0;

			// Look for existing copies of the page
			if(pagePlaced == 0){
				for(k = 0; k < memoryWidth; k++){
					if(memory[k] == sequence[i]){
						pageExists = 1;
					}
				}
			}
			
			// Look for empty memory locations
			if(pageExists == 0){
				for(j = 0; j < memoryWidth; j++){
					if(memory[j] == 0 && pagePlaced == 0){
						memory[j] = sequence[i];
						pagePlaced = 1;
						numberReplacements++;
					}
				}
			}	
			
			// Find replacement place for page
			if(pagePlaced == 0 && pageExists == 0){
				memory[index] = sequence[i];
				if(index < memoryWidth-1){
					index++;
				}
				else{
					index = 0;
				}
				numberReplacements++;
			}
			
		} // End of Page Sequence Iteration
		
		
		reportData[memoryWidth-1][1] = numberReplacements;
	} // End of Memory Width Iteration
}

void *leastRecent(void* arg){

	int *sequence = (int *) arg;
	int memoryWidth = 0;
	int numberReplacements = 0;
	int index = 0;
	int pagePlaced = 0;
	int pageExists = 0;
	int i, j, k, l, m;
	int tempIndex;
	int tempTime;
	int memory[9][2];
	
	for(memoryWidth = 1; memoryWidth < 10; memoryWidth++){
		index = 0;
		// Clear the memory
		for(l = 0; l < memoryWidth; l++){
			memory[l][0] = 0;
			memory[l][1] = 0;
		}
		numberReplacements = 0;
		
		for(i = 0; i < sequenceLength; i++){
			pagePlaced = 0;
			pageExists = 0;
			tempTime = sequenceLength+1;
			tempIndex = 0;

			// Look for existing copies of the page
			if(pagePlaced == 0){
				for(k = 0; k < memoryWidth; k++){
					if(memory[k][0] == sequence[i]){
						pageExists = 1;
						memory[k][1] = i; 				// Time is recorded
					}
				}
			}
			
			// Look for empty memory locations
			if(pageExists == 0){
				for(j = 0; j < memoryWidth; j++){
					if(memory[j][0] == 0 && pagePlaced == 0){
						memory[j][0] = sequence[i];
						memory[j][1] = i; 				// Time is recorded
						pagePlaced = 1;
						numberReplacements++;
					}
				}
			}

			// Find replacement place for page
			if(pagePlaced == 0 && pageExists == 0){
				// Find the least frequently used page
				for(m = 0; m < memoryWidth; m++){
					if(memory[m][1] < tempTime){
						tempTime = memory[m][1];
						tempIndex = m;
					}
				}
				memory[tempIndex][0] = sequence[i]; // Place the page into memory
				memory[tempIndex][1] = i; 			// Set the time
				numberReplacements++;
			}
			
		} // End of Page Sequence Iteration
		
		
		reportData[memoryWidth-1][2] = numberReplacements;
	} // End of Memory Width Iteration
}

void *mostRecent(void* arg){

	int *sequence = (int *) arg;
	int memoryWidth = 0;
	int numberReplacements = 0;
	int index = 0;
	int pagePlaced = 0;
	int pageExists = 0;
	int i, j, k, l, m;
	int tempIndex;
	int tempTime;
	int memory[9][2];
	
	for(memoryWidth = 1; memoryWidth < 10; memoryWidth++){
		index = 0;
		// Clear the memory
		for(l = 0; l < memoryWidth; l++){
			memory[l][0] = 0;
			memory[l][1] = 0;
		}
		numberReplacements = 0;
		
		for(i = 0; i < sequenceLength; i++){
			pagePlaced = 0;
			pageExists = 0;
			tempTime = 0;
			tempIndex = 0;

			// Look for existing copies of the page
			if(pagePlaced == 0){
				for(k = 0; k < memoryWidth; k++){
					if(memory[k][0] == sequence[i]){
						pageExists = 1;
						memory[k][1] = i; 			// Time is recorded
					}
				}
			}
			
			// Look for empty memory locations
			if(pageExists == 0){
				for(j = 0; j < memoryWidth; j++){
					if(memory[j][0] == 0 && pagePlaced == 0){
						memory[j][0] = sequence[i];
						memory[j][1] = i; 			// Time is recorded
						pagePlaced = 1;
						numberReplacements++;
					}
				}
			}

			// Find replacement place for page
			if(pagePlaced == 0 && pageExists == 0){
				// Find the least frequently used page
				for(m = 0; m < memoryWidth; m++){
					if(memory[m][1] > tempTime){
						tempTime = memory[m][1];
						tempIndex = m;
					}
				}
				memory[tempIndex][0] = sequence[i]; // Place the page into memory
				memory[tempIndex][1] = i; 			// Reset the usage count
				numberReplacements++;
			}
		} // End of Page Sequence Iteration
		
		
		reportData[memoryWidth-1][3] = numberReplacements;
	} // End of Memory Width Iteration
}

void *leastFrequent(void* arg){

	int *sequence = (int *) arg;
	int memoryWidth = 0;
	int numberReplacements = 0;
	int index = 0;
	int pagePlaced = 0;
	int pageExists = 0;
	int i, j, k, l, m;
	int tempIndex;
	int tempFreq;
	int memory[9][2];
	
	for(memoryWidth = 1; memoryWidth < 10; memoryWidth++){
		index = 0;
		// Clear the memory
		for(l = 0; l < memoryWidth; l++){
			memory[l][0] = 0;
			memory[l][1] = 0;
		}
		numberReplacements = 0;
		
		for(i = 0; i < sequenceLength; i++){
			pagePlaced = 0;
			pageExists = 0;
			tempFreq = sequenceLength;
			tempIndex = 0;
			
			// Look for existing copies of the page
			if(pagePlaced == 0){
				for(k = 0; k < memoryWidth; k++){
					if(memory[k][0] == sequence[i]){
						pageExists = 1;
						memory[k][1]++; 				// Increment the usage count
					}
				}
			}
			
			// Look for empty memory locations
			if(pageExists == 0){
				for(j = 0; j < memoryWidth; j++){
					if(memory[j][0] == 0 && pagePlaced == 0){
						memory[j][0] = sequence[i];
						pagePlaced = 1;
						numberReplacements++;
					}
				}
			}
			
			// Find replacement place for page
			if(pagePlaced == 0 && pageExists == 0){
				// Find the least frequently used page
				for(m = 0; m < memoryWidth; m++){
					if(memory[m][1] < tempFreq){
						tempFreq = memory[m][1];
						tempIndex = m;
					}
				}
				memory[tempIndex][0] = sequence[i]; 	// Place the page into memory
				memory[tempIndex][1] = 0; 				// Reset the usage count
				numberReplacements++;
			}
		} // End of Page Sequence Iteration
		
		
		reportData[memoryWidth-1][4] = numberReplacements;
	} // End of Memory Width Iteration
}

void *mostFrequent(void* arg){

	int *sequence = (int *) arg;
	int memoryWidth = 0;
	int numberReplacements = 0;
	int index = 0;
	int pagePlaced = 0;
	int pageExists = 0;
	int i, j, k, l, m;
	int tempIndex;
	int tempFreq;
	int memory[9][2];
	
	for(memoryWidth = 1; memoryWidth < 10; memoryWidth++){
		index = 0;
		// Clear the memory
		for(l = 0; l < memoryWidth; l++){
			memory[l][0] = 0;
			memory[l][1] = 0;
		}
		numberReplacements = 0;
		
		for(i = 0; i < sequenceLength; i++){
			pagePlaced = 0;
			pageExists = 0;
			tempFreq = 0;
			tempIndex = 0;

			// Look for existing copies of the page
			if(pagePlaced == 0){
				for(k = 0; k < memoryWidth; k++){
					if(memory[k][0] == sequence[i]){
						pageExists = 1;
						memory[k][1]++; 			// Increment the usage count
					}
				}
			}
			
			// Look for empty memory locations
			if(pageExists == 0){
				for(j = 0; j < memoryWidth; j++){
					if(memory[j][0] == 0 && pagePlaced == 0){
						memory[j][0] = sequence[i];
						pagePlaced = 1;
						numberReplacements++;
					}
				}
			}	
			
			// Find replacement place for page
			if(pagePlaced == 0 && pageExists == 0){
				// Find the least frequently used page
				for(m = 0; m < memoryWidth; m++){
					if(memory[m][1] > tempFreq){
						tempFreq = memory[m][1];
						tempIndex = m;
					}
				}
				memory[tempIndex][0] = sequence[i]; // Place the page into memory
				memory[tempIndex][1] = 0; // Reset the usage count
				numberReplacements++;
			}
		} // End of Page Sequence Iteration
		
		
		reportData[memoryWidth-1][5] = numberReplacements;
	} // End of Memory Width Iteration
}

void *randomSelect(void* arg){ // 6

	int *sequence = (int *) arg;
	int memoryWidth = 0;
	int numberReplacements = 0;
	int index = 0;
	int pagePlaced = 0;
	int pageExists = 0;
	int i, j, k, l, m;
	int randIndex;
	int memory[9];
	
	for(memoryWidth = 1; memoryWidth < 10; memoryWidth++){
		index = 0;
		// Clear the memory
		for(l = 0; l < memoryWidth; l++){
			memory[l] = 0;
		}
		numberReplacements = 0;
		
		for(i = 0; i < sequenceLength; i++){
			pagePlaced = 0;
			pageExists = 0;

			// Look for existing copies of the page
			if(pagePlaced == 0){
				for(k = 0; k < memoryWidth; k++){
					if(memory[k]== sequence[i]){
						pageExists = 1;
					}
				}
			}
			
			// Look for empty memory locations
			if(pageExists == 0){
				for(j = 0; j < memoryWidth; j++){
					if(memory[j] == 0 && pagePlaced == 0){
						memory[j] = sequence[i];
						pagePlaced = 1;
						numberReplacements++;
					}
				}
			}
			
			// Find replacement place for page
			if(pagePlaced == 0 && pageExists == 0){
				randIndex = rand() % memoryWidth;
				memory[randIndex] = sequence[i]; // Place the page into memory
				numberReplacements++;
			}
		} // End of Page Sequence Iteration
		
		
		reportData[memoryWidth-1][6] = numberReplacements;
	} // End of Memory Width Iteration
}

void printReport(int number, int sequence[]){

	int i, j;
	for(i = 0; i < sequenceLength; i++){
		printf("%d ", sequence[i]);
	}
	
	printf("\n\nPage Fault Counts:\n");
	printf("\nW\tOP\tFF\tLR\tMR\tLF\tMF\tRN\n");
	for(j = 0; j < 9; j++){
		printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", (j+1),reportData[j][0],reportData[j][1],reportData[j][2],reportData[j][3],reportData[j][4],reportData[j][5],reportData[j][6]); 
	}
}

void beladyReport(){
	int i, j, k, l, m;
	int current, before;
	int count = 0;
	char label [3];
	
	for(i = 0; i < 7; i++){ // Cycle through each algorithm
		for(j = 0; j < 9; j++){ // Cycle through each width 
			if(j > 0){
				current = reportData[j][i];
				before = reportData[j-1][i];
				if(current > before){
					beladyReportData[count][0] = i;
					beladyReportData[count][1] = j-1;
					beladyReportData[count][2] = j;
					beladyReportData[count][3] = current;
					beladyReportData[count][4] = before;
					beladySummaryData[totalBeladyCount][0] = trialCounter + 1; // Trial Number
					beladySummaryData[totalBeladyCount][1] = i; // Algorithm
					totalBeladyCount++;
					count++;
				}
			}
		}
	}
	
	if(count > 0){
		for(k = 0; k < count; k++){
			printf("Belady Detected : ");
			if(beladyReportData[k][0] == 0){
				strcpy(label, "OP");
			}
			if(beladyReportData[k][0] == 1){
				strcpy(label, "FF");
			}
			if(beladyReportData[k][0] == 2){
				strcpy(label, "LR");
			}
			if(beladyReportData[k][0] == 3){
				strcpy(label, "MR");
			}
			if(beladyReportData[k][0] == 4){
				strcpy(label, "LF");
			}
			if(beladyReportData[k][0] == 5){
				strcpy(label, "MF");
			}
			if(beladyReportData[k][0] == 6){
				strcpy(label, "RN");
			}
		printf(" %s W[%d] = %d < W[%d] = %d (Difference = %d)\n", label, beladyReportData[k][1]+1, beladyReportData[k][4], beladyReportData[k][2]+1, beladyReportData[k][3], (beladyReportData[k][3] - beladyReportData[k][4]));
		}
		printf("\n");
	}
	else{
		printf("\n");
	}
}

void beladySummary(){
	int i, j, k, l;
	int OP = 0;
	int FF = 0;
	int LR = 0;
	int MR = 0;
	int LF = 0;
	int MF = 0;
	int RN = 0;
	int trial, type;
	char label [3];
	
	if(totalBeladyCount > 0){
	
	printf("\nSummary of Belady Anomalies Detected : \n");
		for(i = 0; i < totalBeladyCount; i++){
			trial = beladySummaryData[i][0];
			type = beladySummaryData[i][1];
			if(type == 0){
				strcpy(label, "OP");
				OP++;
			}
			if(type == 1){
				strcpy(label, "FF");
				FF++;
			}
			if(type == 2){
				strcpy(label, "LR");
				LR++;
			}
			if(type == 3){
				strcpy(label, "MR");
				MR++;
			}
			if(type == 4){
				strcpy(label, "LF");
				LF++;
			}
			if(type == 5){
				strcpy(label, "MF");
				MF++;
			}
			if(type == 6){
				strcpy(label, "RN");
				RN++;
			}
			printf("Trial %d: %s\n", trial, label);
		}
	}
	printf("\nBelady's Occurrence Counts:\n");
	printf("OP\tFF\tLR\tMR\tLF\tMF\tRN\n");
	printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t\n", OP, FF, LR, MR, LF, MF, RN);
	
};

void generateSequence(int sequence[]){
	int c, d;
	for(c = 0; c < sequenceLength; c++){
		d = (rand() %9)+1;
		sequence[c] = d;
	}
}

void intializeReportData(){
	int i, j;
	for(i = 0; i < 9; i++){
		for(j = 0; j < 7; j++){
			reportData[i][j] = 0;
		}
	}
}

void intializeBeladyReport(){
	int i, j;
	for(i = 0; i < 63; i++){
		for(j = 0; j < 3; j++){
			beladyReportData[i][j] = 0;
		}
	}
}

void initializeBeladySummary(){
	int i;
	for(i = 0; i < 63; i++){
		beladySummaryData[i][0] = 0;
		beladySummaryData[i][1] = 0;
	}
}



