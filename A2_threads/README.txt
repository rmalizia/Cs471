Russell Malizia
CS 471 
Assignment #2 Threads

**I have modified the existing makefile due to some issues with running multiple commands from the 'all' command. However, the 'make all'
 command has been corrected and should compile all source files as needed.**

TCS Internal Commands:
CTRL-C will print the current list of chatters
CTRL-\ will end the chat server session

TCC Internal Commands:
CTRL-C will connect with the information server and receive list of current chatters
CTRL-D will end the current chat client session

The two source files were written to create two separate programs : TCS (Threaded Chat Server) and TCC (Threaded Chat Client). 
The files have been placed in the rmalizia/cs471w/assignments/a2 folder for your review.

To run the program simply run ./TCS to establish a chat server host (you can specify the port if needed by providing it as an argument, e.g. ./TCS 10999). 

To create client, please run the command ./TCC <localhost> 
(If the <localhost> is not provided it will use "localhost" as default. Furthermore, the port can be specified by providing it as an argument, e.g. ./TCC localhost 10999).




