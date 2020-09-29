#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){
	printf("Parent started\n");
	printf("%s%d%s%d\n", "Parent: getpid() = ", getpid(), ". getppid() = ", getppid());
	
	int N = 2; // Number of children to be created by the parent process.

	/**
	 * Forks child for N number of times.
	 * Child process will exit with status (i+1)*11
	 */
	for(int i=0; i<N; i++){
		pid_t pid = fork();
		if(pid<0){
			fprintf(stderr," Fork Failed\n");
			exit(1);
		}
		else if(pid==0){
			printf("%s%d%s%d%s%d\n", "Child ", i, ": getpid() = ", getpid(), ". getppid() = ", getppid());
			exit(11*(i+1));
		}
	}
	
	/**
	 * Waits for any child to terminate.
	 * Prints the pid and exit status of the child terminated.
	 * Loops until N children are terminated
	 */
	for(int i=0; i<N; i++){
		int status;
		pid_t child = wait(&status);
		printf("%s%d%s%d\n", "child pid ", child, " exited with exit status = ", status);
	}

	return 0;
}
