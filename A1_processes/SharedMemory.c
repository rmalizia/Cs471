#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>



int main(){
	int pid;
	int sgmnt_id;
	char *shrd_mem;
	const int sgmnt_sz = 4096;

	//allocates shared memory
	sgmnt_id = shmget(IPC_PRIVATE, sgmnt_sz), S_IRUSR | S_IWUSR);
	//attached shared memory segment
	shrd_mem = (char *)shmat(sgmnt_id, NULL, 0);
	pid = fork()

	//child process
	if (pid == 0){
		sprintf(shrd_mem, "Hi, brother, I am your sister Alice, my pid is %d\n", getpid());
		sleep(2);
		printf("<Alice>  Got from Bob: %s\n", shrd_mem);

	}
	//parent process
	else {
		printf("\n<Parent> Alice is born, her pid is: %d\n", pid);
		pid = fork();
		if (pid == 0){
			sleep(1)
			printf("<Bob>  Got from Alice: %s\n", shrd_mem);
			sprintf(shrd_mem, "Hi sister, I am your brother Bob, my pid is %d\n", getpid());

		}
		else{
			printf("<Parent> Bob is bornm, his pid is: %d\n", pid);
			sleep(3);
			wait(NULL);
			printf("\n<Parent> Alice is done \n");
			wait(NULL);
			printf("<Parent> Bob is done \n");
			if (shmdt(shrd_mem) == 1){
				fprintf(stderr, "Unable to detach \n");

			}
			schmct1(sgmnt_id, IPC_RMID, NULL);
			return 0;
		
		}
	}
}