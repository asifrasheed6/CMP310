#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int main(){
    for(int i=0; i<2; i++){
        int fileHandle[2];
        pipe(fileHandle);
        
        pid_t pid = fork();
        if(pid==0){
            FILE* ptr = fdopen(fileHandle[1],"w");
            fprintf(ptr,"%f\n", (double) 1000);
            fclose(ptr);
            exit(0);
        }
        else{
            wait(NULL);
            
            FILE* ptr = fdopen(fileHandle[0],"r");
            char input[256];
            
            fscanf(ptr, "%s", input);
            int number = atoi(input);
            fclose(ptr);
            
            printf("%d\n",number);
        }
    }

    return 0;
}
