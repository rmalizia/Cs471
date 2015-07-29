#ifndef _UNISTD_H
#define _UNISTD_H

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(int argc, char *argv[]){

	int             n;
	int             i;
	int             sfd, alice_fd, bob_fd;
	pid_t           pid;
	char            buf[1024];

	//open a file, or create a read/write file
	if ((sfd = open("TempFile", O_RDWR | O_TRUNC | O_CREAT, 0600)) < 0) {
		perror("TempFile");
		exit(1);
	}
	
	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	}
	
	if (pid == 0) {
		if ((alice_fd = open("TempFile", O_RDWR)) < 0) {
			perror("TempFile");
			exit(1);
		}
		
		char Alice_msg[1024];
		int s = sprintf(Alice_msg, "Hi brother, I am your sister Alice, my pid is %d\n", getpid());
		
		write(alice_fd, Alice_msg, s);
		sleep(2);
		lseek(alice_fd, s, SEEK_SET);  //reposition read/write file offset
		n = read(alice_fd, buf, sizeof(buf));
		printf("\n<Alice>  Got from Bob: %s", buf);
		exit(0);
	}
	else {
		printf("\n<Parent> Alice is born, her pid is: %d\n", pid);
		if ((pid = fork()) < 0) {
			perror("fork");
			exit(1);
		}
		if (pid == 0) {
			if ((bob_fd = open("TempFile", O_RDWR)) < 0) {
				perror("TempFile");
				exit(1);
			}
			char Bob_msg[1024];
			int s = sprintf(Bob_msg, "Hi sister, I am your brother Bob, my pid is %d\n", getpid());
			sleep(1);
			n = read(bob_fd, buf, sizeof(buf));
			write(bob_fd, Bob_msg, s);
			printf("<Bob>  Got from Alice: %s", buf);
			exit(0);
		}
		else{
			printf("<Parent> Bob is born, his pid is: %d\n\n", pid);
			sleep(3);
			wait(NULL);
			printf("\n<Parent> Alice is done\n");
			wait(NULL);
			printf("<Parent> Bob is done\n\n");
			execlp("/bin/rm", "rm", "-f", "TempFile", NULL);
			exit(0);
		
		}
	}
}