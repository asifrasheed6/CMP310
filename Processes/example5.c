#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(){
    int fileHandle[2];
    pipe(fileHandle);
    
    pid_t pid = fork();
    if(pid==0){
        FILE* ptr = fdopen(fileHandle[1],"w");
        fprintf(ptr,"%s\n","The quick brown fox jumps over the lazy dog");
    }
    
    wait(NULL);
    
    FILE* ptr = fdopen(fileHandle[0],"r");
    char input[256];
    fscanf(ptr, "%s", input);
    
    printf(input);
    
    exit(0);
    
    return 0;
}
