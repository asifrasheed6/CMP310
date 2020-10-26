#include<stdio.h>
#include<QThread>
#include<QMutex>
#include<QSemaphore>

/*
 Dining Philosophers problem approach 1: Only take fork if both are available.
 */

class Philisopher : public QThread{
private:
    int id;
public:
    static QSemaphore chopsticks[5];
    
    Philisopher(int id) : id(id) {}
    
    void run(){
        while(1){
            if(chopsticks[id].available() && chopsticks[(id+1) % 5].available()){
                chopsticks[id].acquire();
                chopsticks[(id+1) % 5].acquire();
                printf("Philosopher %d is eating...\n", id);
                sleep(rand()%5);
                printf("Philosopher %d is done eating\n", id);
                chopsticks[id].release();
                chopsticks[(id+1) % 5].release();
                break;
            }
        }
    }
};
QSemaphore Philisopher::chopsticks[5];

int main(){
    Philisopher *philosophers[5];
    
    for(int i=0; i<5; i++) Philisopher::chopsticks[i].release();
    
    for(int i=0; i<5; i++){
        philosophers[i] = new Philisopher(i);
        philosophers[i]->start();
    }
    
    for(int i=0; i<5; i++){
        philosophers[i]->wait();
        delete philosophers[i];
    }
    
    return 0;
}
