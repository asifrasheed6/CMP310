#include<stdio.h>
#include<stdlib.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<fcntl.h>

// Creating a shared memory for processes using int to repesent the file handler.
// One of the three methods for file I/O in C++ (using int, using streams and using pointer to file structure).

int main(){
    int shm_fd = shm_open("asif", O_CREAT|O_RDWR, 0666); // The first parameter is a unique string to identify the file, the second parameter are the options (O_CREATE will create the file, O_RDWR will open th file) and the third parameter is the permission in octal (therefore the first 0 is important).
    ftruncate(shm_fd, 1024); // Assigning 1 Gib of memory for the shared memory space. This could be less than or greater than the actual file memory.
    void* ptr = mmap(0, 1024, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0); // Maps to memory
    
    sprintf(ptr, "The quick brown fox jumps over the lay dog");
    
    return 0;
}
