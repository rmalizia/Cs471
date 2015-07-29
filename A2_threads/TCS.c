#include "def.h"
#define SIZE 10;

int connfd[10];
char info[10][512];
int chatPort = 10184; //Default port numbers
int infoPort = 10185;

void exit();
void handleChatters(int);
void sigHandler(int);
void informationServer();
void postGoodBye();

int main(int argc, char *argv[]){

    int len, listenfd, infofd;
    struct sockaddr_in  servaddr, cliaddr;
    char buff[512];
	char hostName[512];
    int nread;
	int i, a;
    pthread_t tid, tid2;
    pthread_attr_t attr;
	
	// Declare signals for interrupt handling
	signal(SIGINT, sigHandler);
	signal(SIGQUIT, sigHandler);
	
	if (argc == 2){ //Takes a different port number if declared by the user
		chatPort = atoi(argv[1]);
		infoPort = chatPort + 1;
	}

	for(a = 0; a < 10; a++){
		connfd[a] = -1;
	}

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) { //Create socket for server
        printf ("Cannot Create Chat Server Socket\n");
        exit(-1);
    }

    //Create socket for Chat Server
	bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(chatPort);

	//Binding chat socket
    if(bind(listenfd, (SA *) & servaddr, sizeof(servaddr)) < 0) {
        printf("Cannot Bind Chat Server Socket\n");
        exit(-1);
    }

    gethostname(hostName, 511);

	listen(listenfd,0);
	printf("\nChat Server Started on Server : %s Port : %d\n", hostName, servaddr.sin_port);
	pthread_attr_init(&attr);
	pthread_create(&tid2, &attr, &informationServer, NULL);
	
	// Wait for chat clients to join
    for (;;) {
		for(i = 0; i < 10; i++) {
			if(connfd[i] == -1){
				len = sizeof(cliaddr);
				connfd[i] = accept(listenfd, (SA *) & cliaddr, &len);
				printf("New Chat Connection from: %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
				sprintf(info[i], "%s:%d", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port)); //The client server info is added to the stored array
				pthread_create(&tid, &attr, &handleChatters, i); //Create a new thread each time a client is added
			}
		}
	}
}

void handleChatters(int index){

    int nread;
    char buffer[512];
	int i;

    for (;;) {
		
		nread = read(connfd[index], buffer, 511); //Wait for new messages
		
		if(nread <= 0){ //Determine if any of the chatters leave and delete them from the list
			connfd[index] = -1;
			close(connfd[index]);
			pthread_exit(0);
		}

		for(i = 0; i < 10; i++) { //Send received message out to all of the clients
			if(connfd[i] != -1){
			write(connfd[i], buffer, 511);
			}
		}
		memset(buffer, '\0', 511); //Clear buffer for next message
	}
}

void informationServer(){

	struct sockaddr_in  infoservaddr, infocliaddr;
	int askfd, infofd, i, len;
	
	// Create a socket for the chatter list server
	if((infofd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
        printf ("Cannot Create Info Server Socket\n");
        exit(-1);
    }

	bzero(&infoservaddr, sizeof(infoservaddr));
    infoservaddr.sin_family = AF_INET;
    infoservaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    infoservaddr.sin_port = htons(infoPort);

	
    if(bind(infofd, (SA *) & infoservaddr, sizeof(infoservaddr)) < 0){
        printf("Cannot Bind Info Server Socket\n");
        exit(-1);
    }
	
	len = sizeof(infocliaddr);
	
    listen(infofd, 0); //Wait for clients to join the server
	for(;;){
		askfd = accept(infofd, (SA *) & infocliaddr, &len); //Accept the new connection
		printf("%s:%d asked for list of chatters\n", inet_ntoa(infocliaddr.sin_addr), ntohs(infocliaddr.sin_port));
		for(i = 0; i < 10; i++){ //Print out the list of current chatters
			if(connfd[i] != -1){
				write(askfd,info[i],511);
			}
		}
		close(askfd);
	}
}

void postGoodBye(){
	
	char buffer[512];
	int i;
	
	printf("Chat Session Ending!\n");
	sprintf(buffer, "\nServer closing! Bye, Bye!\n");
	
	for(i = 0; i < 10; i++) { //Print salutation to all clients and close all connections
		if(connfd[i] != -1){
		write(connfd[i], buffer, strlen(buffer));
		close(connfd[i]);
		}
	}
	
}
void sigHandler (int sig){ //Handle all signals
	
	int i;
	signal(sig, sigHandler);
	
	printf("\nsigHandler:: Signal Received: ");
	if(sig == 2){ //Determine is interrupt sent
		printf("Interrupt\n");
		printf("List of Chatters:\n"); 
		for(i = 0; i < 10; i++) { // Print current list of chatters to terminal
			if(connfd[i] != -1){
				printf("%s\n", info[i]);
			}
		}
	}
 
	if(sig == 3){ //End Chat Server Session
		printf("Quit\n");
		postGoodBye();
		exit(0);
	}
}

