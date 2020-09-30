#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>

int main(int argc, char** argv){
	if(argc==1){
		printf("Usage ./program [argument]\n");
		exit(1);
	}
    
    int SIZE = strlen(argv[1]);
    
	int fd = shm_open("asif", O_CREAT|O_RDWR, 0666);
	ftruncate(fd, SIZE);
	void* ptr = mmap(0, SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
	sprintf(ptr, argv[1]);

	unsigned char* f = (char *) ptr;
	for(int i=0; i<SIZE; i++){
		char out = f[i];
		printf("%c\n",out);
	}
        
	return 0;
}
