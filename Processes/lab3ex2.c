#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>

int main(){
	printf("Parent Started\n");
	printf("%s%d%s%d\n","Parent: getpid() = ",getpid(),". getppid() = ",getppid());

	for(int i=0;i<2;i++){
		pid_t pid = fork();

		if(pid<0){
			fprintf(stderr," Fork Failed\n");
			exit(1);
		}
		else if(pid == 0){
			printf("%s%d%s%d%s%d\n","Child ",i,": getpid() = ",getpid(),". getppid() = ",getppid());
			exit(11*(i+1));
		}
		else{
			int status;
			waitpid(pid,&status,0);
			printf("%s%d%s%d\n","Child pid ",pid," exited with exit status = ",status);
		}
	}

	return 0;
}

