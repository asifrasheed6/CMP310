#include<stdio.h>
#include<QThread>
#include<QMutex>

QMutex humus, tabouli, shish, lamb;

class Man : public QThread{
public:
    void run(){
        printf("Man  Humus\n");
        humus.lock();
        //(rand() % 3);
        humus.unlock();
        
        printf("Man Tabouli \n");
        tabouli.lock();
        //sleep(rand() % 3);
        tabouli.unlock();
        
        printf("Man Shish \n");
        shish.lock();
        //sleep(rand() % 3);
        shish.unlock();
        
        printf("Man Lamb \n");
        lamb.lock();
        //sleep(rand() % 3);
        lamb.unlock();
    }
};

class Women : public QThread{
public:
    void run(){
        printf("Women Tabouli \n");
        tabouli.lock();
        //sleep(rand() % 3);
        tabouli.unlock();
        
        printf("Women  Humus\n");
        humus.lock();
        //sleep(rand() % 3);
        humus.unlock();
        
        printf("Women Lamb \n");
        lamb.lock();
        //sleep(rand() % 3);
        lamb.unlock();
        
        printf("Women Shish \n");
        shish.lock();
        //sleep(rand() % 3);
        shish.unlock();
    }
};


int main(){
    Man man[5];
    Women women[5];
    
    for(int i=0; i<5; i++){
        man[i].start();
        women[i].start();
    }
    
    for(int i=0; i<5; i++){
        man[i].wait();
        women[i].wait();
    }
    
    return 0;
}
