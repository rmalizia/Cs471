#ifndef _UNISTD_H
#define _UNISTD_H

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(int argc, char *argv[]){

	int n;
	int i;
	int sfd;
	pid_t pid;
	char buf[1024];

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
		char Alice_msg[1024];
		int s = sprintf(Alice_msg, "Hi brother, I am your sister Alice, my pid is %d\n", getpid());
		write(sfd, Alice_msg, s);
		sleep(3);
		lseek(sfd, s, SEEK_SET);  //reposition read/write file offset
		n = read(sfd, buf, sizeof(buf));
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
			char Bob_msg[1024];
			int s = sprintf(Bob_msg, "Hi sister, I am your brother Bob, my pid is %d\n", getpid());
			sleep(1);
			lseek(sfd, 0, SEEK_SET);  //reposition read/write file offset
			n = read(sfd, buf, sizeof(buf));
			write(sfd, Bob_msg, s);
			printf("<Bob>  Got from Alice: %s", buf);
			exit(0);
		}
		else{
			printf("<Parent> Bob is born, his pid is: %d\n\n", pid);
			sleep(4);
			wait(NULL);
			printf("\n<Parent> Alice is done\n");
			wait(NULL);
			printf("<Parent> Bob is done\n\n");
			execlp("/bin/rm", "rm", "-f", "TempFile", NULL);
			exit(0);
		
		}
	}
}