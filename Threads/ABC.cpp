#include<stdio.h>
#include<QThread>
#include<QSemaphore>

class ThreadB : public QThread{
private:
    QSemaphore *semaphoreB, *semaphoreAC, *total;
public:
    
    ThreadB(QSemaphore *semaphoreB, QSemaphore *semaphoreAC, QSemaphore *total) : semaphoreB(semaphoreB), semaphoreAC(semaphoreAC), total(total) {}
    
    void run(){
        while(1){
            semaphoreB->acquire();
            if(!total->tryAcquire()){
                semaphoreAC->release(2);
                break;
            }
            printf("B");
            semaphoreAC->release(4);
        }
    }
};

class ThreadA : public QThread{
private:
    QSemaphore *semaphoreB, *semaphoreAC, *total;
public:
    
    ThreadA(QSemaphore *semaphoreB, QSemaphore *semaphoreAC, QSemaphore *total) : semaphoreB(semaphoreB), semaphoreAC(semaphoreAC), total(total) {}
    
    void run(){
        while(1){
            semaphoreAC->acquire();
            if(!total->tryAcquire()) break;
            printf("A");
            
            if(semaphoreAC->available()==0 && semaphoreB->available() == 0)
                semaphoreB->release();
        }
    }
};

class ThreadC : public QThread{
private:
    QSemaphore *semaphoreB, *semaphoreAC, *total;
public:
    
    ThreadC(QSemaphore *semaphoreB, QSemaphore *semaphoreAC, QSemaphore *total) : semaphoreB(semaphoreB), semaphoreAC(semaphoreAC), total(total) {}
    
    void run(){
        while(1){
            semaphoreAC->acquire();
            if(!total->tryAcquire()) break;
            printf("C");
            
            if(semaphoreAC->available()==0 && semaphoreB->available() == 0)
                semaphoreB->release();
        }
    }
};

int main(){
    QSemaphore semaphoreB(1), semaphoreAC, total(100);
    
    ThreadA threadA(&semaphoreB, &semaphoreAC, &total);
    ThreadB threadB(&semaphoreB, &semaphoreAC, &total);
    ThreadC threadC(&semaphoreB, &semaphoreAC, &total);
    
    threadB.start();
    threadA.start();
    threadC.start();
    
    threadB.wait();
    threadA.wait();
    threadC.wait();
    
    printf("\n");
    
    return 0;
}
