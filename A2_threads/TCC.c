#include "def.h"

void ReadFromKeyBoard();
void ReadFromSocket();
void sigHandler(int);
void exit();
int sockfd, getinfofd;
int chatPort = 10184;
int infoPort = 10185;
char host[512] = "localhost";   //default local host value

int main(int argc, char *argv[]){
   
	signal (SIGINT, sigHandler); //Interrupt signal is 'overloaded' to give functionality 
    pthread_t tid;
    pthread_attr_t attr;
    struct sockaddr_in servaddr;
    struct hostent *hp, *gethostbyname();

    if (argc == 2){             //Determine if a local host been declared
		sprintf(host, argv[1]);
	}
	
    if (argc == 3){             //Determine if a port number has been specified
		chatPort = atoi(argv[2]);
		infoPort = chatPort + 1;
	}
	
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){// Establish socket
	    printf ("Cannot Create Chat Client Socket\n");
        exit(-1);
	}; 
	
    bzero(&servaddr, sizeof(servaddr));
    hp = gethostbyname(host);
    bcopy(hp->h_addr, &(servaddr.sin_addr.s_addr), hp->h_length);
	servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(chatPort);

    if (connect(sockfd, (SA *) & servaddr, sizeof(servaddr)) < 0) { //Connect with the Chat Host Server
        perror("Chat Server Connection Error");
        exit(-1);
    }
    pthread_attr_init(&attr);
	pthread_create(&tid, &attr, &ReadFromKeyBoard, sockfd); //Make thread to read input from the keyboard
    ReadFromSocket(sockfd);         //Read messages coming from the server
}

void ReadFromKeyBoard(int sockfd){
	
    int nread;
    char buffer[512];
	
    for (;;) {
        nread = read(0, buffer, 511);
        
        if (nread == 0){            //If the ctrl-D is command is entered then this value becomes 0 and the Client exits
            printf("\nExited from Chat Server!\n");
			exit(0);
		}
        write(sockfd, buffer, 511); //Write message to the server
		
		memset(buffer, '\0', 511); //Clear buffer for next input
    }
}

void ReadFromSocket(int sockfd){

    int nread;
    char buffer[512];
	
    for (;;) {
        nread = read(sockfd, buffer, 511);
        if (nread == 0)
            break;
		
        printf("%s", buffer);       //Print messages from the server to the screen
		memset(buffer, '\0', 511);
    }
}

void sigHandler(int sig){
	
	char infobuf[512];
	int inforead;
    signal(sig,sigHandler);         //Reset the signal
	
	printf("\nsigHandler:: Signal Received: Interrupt\n");
	

    //Establish Connections needed with the Information Server
	struct sockaddr_in infoservaddr;
    struct hostent *hp, *gethostbyname();
	getinfofd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&infoservaddr, sizeof(infoservaddr));
    hp = gethostbyname(host);
    bcopy(hp->h_addr, &(infoservaddr.sin_addr.s_addr), hp->h_length);

    infoservaddr.sin_family = AF_INET;
    infoservaddr.sin_port = htons(infoPort);

    if (connect(getinfofd, (SA *) & infoservaddr, sizeof(infoservaddr)) < 0) {
        perror("Info Server Connection Error");
        exit(-1);
    }
	
	printf("List of Current Chatters : \n");
	for(;;){
		inforead = read(getinfofd, infobuf, 511);
		if(inforead == 0){
			break;
		}
        printf("%s\n",infobuf);         // Print the list of current chatters
	}
	
	close(getinfofd);

}
