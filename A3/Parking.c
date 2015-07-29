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

struct Car{

	int id;
	int randArrivalTime;
	int departTime;
	int parkTime;
	int randParkTime;
	int waitTime;
	int parkingSpaceNumber;
};

struct ParkingSpot{
	
	int id;
	int cars[100][2];
	int occupied;

};

pthread_mutex_t ParkingLot;
pthread_mutex_t ParkingSpaceCount;
sem_t LotEmpty;

//CASP
int numberOfCars;
int carArrivalTime;
int numberOfSpaces;
int timeToPark;

int emptySpaces;
int beginningOfTime;
int endOfTime;
int departedCars;
int totalRunTime;

struct Car carArray[100];
struct ParkingSpot parkingLotArray[100];

void *carThread(void *arg);
void initialization();
void outputStatements();



void main(int argc, char *argv[])
{
	pthread_t newCarThread;
	pthread_attr_t carAttr;


	
	if(argc == 1){
		numberOfCars = 3; //use default value
		carArrivalTime = 3;
		numberOfSpaces = 2;
		timeToPark = 20;
		printf("Using default values: %d cars, Random Arrival Time %d Seconds\n\t\t%d Spaces, Random Parking Time %d Seconds\n",numberOfCars, carArrivalTime, numberOfSpaces, timeToPark);
	}

	else if(argc == 5){
		if (atoi(argv[1])<=100){
			numberOfCars = atoi(argv[1]);
		}
		else{
			numberOfCars = 100;
		}
		if(atoi(argv[2])<=100){
			carArrivalTime = atoi(argv[2]);
		}
		else{
			carArrivalTime = 100;
		}
		if(atoi(argv[3])<=100){
			numberOfSpaces = atoi(argv[3]);
		}
		else{
			numberOfSpaces = 100;
		}
		if(atoi(argv[4])<=100){
			timeToPark = atoi(argv[4]);
		}
		else{
			timeToPark = 100;
		}
	}
	else{
		//error mesaages
		printf("Usage : Parking[C A S P]\n");
		printf("\tC - Number of Cars\n");
		printf("\tA - Random Time Between Car Arrivals\n");
		printf("\tS - Number of Parking Spaces\n");
		printf("\tP - Random Time to Park\n");
        printf("Maximum of 100 for each input value\n");
		exit(0);
	}

	beginningOfTime = time(NULL);
	//Initialize data structures
    initialization();
	
	printf("... PARKING LOT OPEN ...\n");
	printf("Expecting (%d) Cars with Random Arrival Times (%d seconds)\n ... Parking spaces (%d) with Random Parking Time (%d seconds)\n\n",numberOfCars, carArrivalTime, numberOfSpaces, timeToPark);

	sem_init(&LotEmpty,0,0);

	int i, randArrival;

	pthread_attr_init(&carAttr);
	pthread_mutex_init(&ParkingLot, NULL);
    pthread_mutex_init(&ParkingSpaceCount, NULL);

    for(i = 0; i < numberOfCars; i++){   //for loop to create cars and wait random time
		
		//sleep for random arrival amount of time between each car 
        randArrival = (rand() %carArrivalTime)+1;
		sleep(randArrival);
		pthread_create(&newCarThread, &carAttr, carThread, (void *) i);
	}

	//wait for the finish semaphor
	sem_wait(&LotEmpty);
	outputStatements();
	//exit after the finish semaphor is activated
	exit(0);
}

void *carThread(void *arg)
{
	int index = (int)arg;
	int i,j;
    int parkedIndicator = 0;
	int recorded = 0;
	
    carArray[index].randArrivalTime = time(NULL) - beginningOfTime; //determine arrival time
    printf("C%d arrived at %d seconds... ", carArray[index].id, carArray[index].randArrivalTime); //print car arrival time
	//check for empty spaces
	if(emptySpaces == 0){
		printf("Parking is FULL\n");
	}
	else{
		printf("Available Spaces : %d\n", emptySpaces);
	}
    while(parkedIndicator == 0){
		pthread_mutex_lock(&ParkingLot); //lock parking lot

		for(i =0; i < numberOfSpaces; i++){  //iterate through spaces
			if(parkingLotArray[i].occupied == 0 && parkedIndicator ==0){
				carArray[index].randParkTime = time(NULL) - beginningOfTime;
				carArray[index].waitTime = carArray[index].randParkTime - carArray[index].randArrivalTime;
                printf("C%d Parked in S%d after waiting for the %d seconds\n", carArray[index].id, i+1, carArray[index].waitTime);
				
				pthread_mutex_lock(&ParkingSpaceCount);
				--emptySpaces;
				
                pthread_mutex_unlock(&ParkingSpaceCount);
				
				parkingLotArray[i].occupied = 1;
                carArray[index].parkingSpaceNumber = i+1;
                carArray[index].randParkTime = (rand() % timeToPark) + 1;
				j=0;
				while(recorded == 0){
					if(parkingLotArray[i].cars[j][0] == 0){
						parkingLotArray[i].cars[j][0]= carArray[index].id;
                        parkingLotArray[i].cars[j][1]= carArray[index].randParkTime;
						recorded = 1;
					}
					j++;
				}
				parkedIndicator = 1;
			}
		}
        pthread_mutex_unlock(&ParkingLot); //unlock parking lot
        if(parkedIndicator == 0){
			sleep(1);
		}
	}
    sleep(carArray[index].randParkTime);
	//update structs and determine departure time 
	carArray[index].departTime = time(NULL) - beginningOfTime;
	
	pthread_mutex_lock(&ParkingLot);
    parkingLotArray[carArray[index].parkingSpaceNumber-1].occupied = 0;
	
	pthread_mutex_unlock(&ParkingLot);
	pthread_mutex_lock(&ParkingSpaceCount);
	++emptySpaces;

	pthread_mutex_unlock(&ParkingSpaceCount);
	
	++departedCars;
	
	//output that a car departed
    printf("C%d left after parking %d seconds \n", carArray[index].id, carArray[index].randParkTime);
	//have all of the cars left?
	if(departedCars == numberOfCars){
		sem_post(&LotEmpty);
		endOfTime = time(NULL);
		totalRunTime = endOfTime - beginningOfTime;
	}
	
}

void outputStatements()
{
    int i, j;
	printf("\n ... PARKING LOT CLOSED ... Number of Parked Cars %d\n", departedCars);
	printf("\n=================== FINAL STATE ======================\n\n");
	printf("Total Time from Open to Close : %d seconds\n\n", totalRunTime);
	printf("Parking Spaces :\n");
	printf("----------------\n");
	printf("\tCar\tParking Time\n");
	for(i = 0; i < numberOfSpaces; i++){
		printf("S%d:\n", parkingLotArray[i].id);
        for(j=0; j < 100; j++){
			if(parkingLotArray[i].cars[j][0] !=0){
				printf("\tC%d\t%d\n",parkingLotArray[i].cars[j][0], parkingLotArray[i].cars[j][1]);
			}
		}
	}
	int id;
	int cars[100][2];
	int occupied;
	
	printf("\n\nCars : \n");
	printf("-----------\n");
	printf("\tSpace\tArrival Time\tWaitingTime \tParking Time\n");
	for(i = 0; i < numberOfCars; i++){
        printf("C%d:\t%d\t%d\t\t%d\t\t%d\n", carArray[i].parkingSpaceNumber, carArray[i].randArrivalTime, carArray[i].waitTime, carArray[i].randParkTime);
	}
	printf("\n");
}

void initialization()
{
	int i, j;
	
	//seed the random time with pid
    srand(time(NULL) + getpid() * 42);
	//set empty spaces
	emptySpaces = numberOfSpaces;
    departedCars = 0;
    for(i = 0; i < 100; i++){
		carArray[i].id = i+1;
        carArray[i].randArrivalTime = 0;
		carArray[i].departTime = 0;
		carArray[i].parkTime = 0;
        carArray[i].randParkTime = 0;
		carArray[i].waitTime = 0;
        carArray[i].parkingSpaceNumber = 0;
	
	}
    for(i = 0; i < 100; i++){
		parkingLotArray[i].id = i+1;
		parkingLotArray[i].occupied = 0;
		for(j = 0; j < 100; j++){
			parkingLotArray[i].cars[j][0] = 0;
			parkingLotArray[i].cars[j][1] = 0;
		}
	}
}













