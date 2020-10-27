#include<stdio.h>
#include<QThreads>
#include<QSemaphore>

class ThreadB : public QThreads{
private:
    QSemaphore *semaphoreB, *semaphoreAC, *total;
public:
    
    ThreadB(QSemaphore *semaphoreB, QSemaphore *semaphoreAC, QSemaphore *total) : semaphoreB(semaphoreB), semaphoreAC(semaphoreAC), total(total) {}
    
    void run(){
        
    }
};

class ThreadA : public QThreads{
private:
    QSemaphore *semaphoreB, *semaphoreAC, *total;
public:
    
    ThreadA(QSemaphore *semaphoreB, QSemaphore *semaphoreAC, QSemaphore *total) : semaphoreB(semaphoreB), semaphoreAC(semaphoreAC), total(total) {}
    
    void run(){
        
    }
};

class ThreadC : public QThreads{
private:
    QSemaphore *semaphoreB, *semaphoreAC, *total;
public:
    
    ThreadC(QSemaphore *semaphoreB, QSemaphore *semaphoreAC, QSemaphore *total) : semaphoreB(semaphoreB), semaphoreAC(semaphoreAC), total(total) {}
    
    void run(){
        
    }
};
