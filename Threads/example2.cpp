#include<iostream>
#include<QThread>
/*  There is something else called as QMutexLocker because in case if you forget to unlock a thread, it will lead to deadlock. QMutexLocker requires a QMutex object, the constructiore of QMutexLocker need a reference to an mutex locker and then it automatically locks the mutex, now once the object is destroyed, the destructor will unlock the mutex. For example:
    {
        QMutexLocker locker(&mutex); <- mutex is locked here by the constructor
    } <- mutex is unlocked here by the destructor
 */
#include<QMutex>
using namespace std;

QMutex mutex1; // It has two states on/off. This is declared globally and therefore is shared among all the threads

class MyThread : public QThread{ // Class to implement run block for thread, similar approach to Java (Keep in mind that this is not the POSIX way of doing things)
private:
    int ID; // Thread ID
    
public:
    MyThread(int id) : ID(id) {}
    void run() {
        mutex1.lock(); // We lock the mutex here, we cannot lock it twice. The function will goto idle state if the function tries to lock a mutex which is already locked until some other thread unlocks it
        cout << "Thread " << ID << "is running\n"; // Every time a part of this code is executed by the CPU, it tries to context switch. So when it does that, it won't be able to do that because all the other threads are locked. This is the critical section
        mutex1.unlock();
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

