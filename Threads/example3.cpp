#include<iostream>
#include<QThread>
#include<QSemaphore> // More flexible way compared to Mutex, in mutex only one thread is allowed access to the critical section once locked. Semaphore constructor can take positive values and that many threads are allowed into the critical section
using namespace std;

QSemaphore semaphore(2); // 2 threads are allowed into the critical section at a time

class MyThread : public QThread{ // Class to implement run block for thread, similar approach to Java (Keep in mind that this is not the POSIX way of doing things)
private:
    int ID; // Thread ID
    
public:
    MyThread(int id) : ID(id) {}
    void run() {
        semaphore.acquire(); // sem--, when sem value is 0 and a thread attempts to acquire, it will goto an idle state
        cout << "Thread " << ID << "is running\n";
        semaphore.release(); // sem++
    }
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

