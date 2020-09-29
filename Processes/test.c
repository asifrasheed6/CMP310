#include<stdio.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>

int main(){
	int fd = shm_open("asif",O_RDWR,0666);
    ftruncate(fd,44);
	void* ptr = mmap(0,44,PROT_READ,MAP_SHARED,fd,0);
	char* string = (char*) ptr;
	printf(string);
	return 0;
}
