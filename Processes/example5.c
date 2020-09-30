#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int main(){
    int fileHandle[2];
    pipe(fileHandle);
    
    pid_t pid = fork();
    if(pid==0){
        FILE* ptr = fdopen(fileHandle[1],"w");
        fprintf(ptr,"%s\n","10000");
        
        exit(0);
    }
    
    wait(NULL);
    
    FILE* ptr = fdopen(fileHandle[0],"r");
    char input[256];
    
    fscanf(ptr, "%s", input);
    int number = atoi(input);
    
    printf("%d\n",number);
    
    return 0;
}
