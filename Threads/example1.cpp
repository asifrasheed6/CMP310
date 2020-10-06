#include<iostream>
#include<QThread>
using namespace std;

class MyThread : public QThread{ // Class to implement run block for thread, similar approach to Java (Keep in mind that this is not the POSIX way of doing things)
private:
    int ID; // Thread ID
    
public:
    MyThread(int id) : ID(id) {}
    void run() { cout << "Thread " << ID << "is running\n"; }
};

int main(int argc, char** argv){
    MyThread* threads[10];
    
    for(int i=0;i<10;i++){
        threads[i] = new MyThread(i);
        threads[i]->start(); // To start thread execution
    }
    
    for(int i=0; i<10; i++)
        threads[i]->wait(); // In order for the parent thread to wait for a child thread
    
    return 0;
}
