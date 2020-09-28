#include<iostream>
#include<unistd.h>
#include<signal.h> // Just for line 24
using namespace std;

// Fork and Fork Bombs

int main(int argc, char** argv){ // Argument counter and Argument Vector
    if(argc==1){ // If there are no commandline parameters passed
        cout<<argv[0]<<" loopN\n";
        exit(1);
    }
    
    int N = atoi(argv[1]); // ascii to integer
    
    for(int i=0;i<N;i++){
        pid_t child = fork();
        if(child!=0) // If it is the parent
            cout<<getpid()<<" Creared Child "<<child<<endl; // This would be printed 2^N-1 times
    }
    
    // kill(pid,sig) will kill the particular process id. Value for SIGKILL is 9. It is defined in Signal.h, so better to use that.
    // exit(status) will return the status value to the parent by reference where the wait() is called with the help of signals.
    cout<<"The Signal for Kill is "<<SIGKILL<<endl;
    
    return 0;
}
