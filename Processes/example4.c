#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>

int main(){
	int fd = shm_open("asif",O_CREAT,0666);
	ftruncate(fd,1024);
	void* ptr = mmap(0,1024,PROT_WRITE,MAP_SHARED,fd,0);
	sprintf(ptr,"The quick brown fox jumps over the lazy dog");
	return 0;
}
