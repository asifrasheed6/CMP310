#include<iostream>
#include<unistd.h>
using namespace std;

// Introduction

int main(){
    pid_t pid; // Special type which is esentially an integer
    pid = fork();
    
    if(pid<0){ // If there is an error
        cout<<stderr<<" Fork Failed"<<endl; // Standard Error is the default file descriptor where a process could write error messages. File descriptor is a number that uniquely identifies an open file in an operating system.
        return 1;
    }
    else if(pid==0) // If it is the child
        execlp("/bin/ls","ls",NULL); // The function only returns a value if there is an error.
    else{ // If it is the parent
        wait(NULL); // Waits until any child created by the parent terminates. So even if there are multiple children, it will only wait for one. If you want it to wait for all the children, you need a for loop. If you want to wait for a specific child, the you could use waitpid. There is another call called system() which would create another child but then it is a blocking code, so it will wait for the child to return. If fork and exec is called, then the child could work independent of the parent.
        cout<<"Child complete"<<endl;
    }

    return 0;
}

/*
 There are 5 system calls in total in this example:
    - In line number 7: fork
    - In line number 10: cout
    - In line number 14: execlp
    - In line number 16: wait
    - In line number 17: cout
 
 Exec is a collection of functions. There are two types of exec functions, execl and execv, the differe in the way we specify the commandline parameters.
 */
