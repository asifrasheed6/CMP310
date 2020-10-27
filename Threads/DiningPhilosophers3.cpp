#include<stdio.h>
#include<QThread>
#include<QMutex>
#include<QSemaphore>

/*
 Dining Philosophers problem approach 3: An odd numbered philosopher would take the copsticks first and then an even numbered philosopher.
 */

class Philosopher : public QThread{
private:
    int id;
public:
    static QSemaphore chopsticks[5];
    static QSemaphore philosopher;
    
    Philosopher(int id) : id(id) {}
    
    void run(){
        if((id+1) % 2 == 1){
            chopsticks[id].acquire();
            chopsticks[(id+1) % 5].acquire();
            printf("Philosopher %d is eating...\n", id);
            sleep(rand() % 5);
            printf("Philosopher %d is done eating...\n", id);
            chopsticks[id].release();
            chopsticks[(id+1) % 5].release();
            philosopher.release();
        }
        else{
            philosopher.acquire();
            chopsticks[id].acquire();
            chopsticks[(id+1) % 5].acquire();
            printf("Philosopher %d is eating...\n", id);
            sleep(rand() % 5);
            printf("Philosopher %d is done eating...\n", id);
            chopsticks[id].release();
            chopsticks[(id+1) % 5].release();
        }
    }
};
QSemaphore Philosopher::chopsticks[5];
QSemaphore Philosopher::philosopher;

int main(){
    Philosopher *philosophers[5];
    
    for(int i=0; i<5; i++) Philosopher::chopsticks[i].release();
    
    for(int i=0; i<5; i++){
        philosophers[i] = new Philosopher(i);
        philosophers[i]->start();
    }
    
    for(int i=0; i<5; i++){
        philosophers[i]->wait();
        delete philosophers[i];
    }
    
    return 0;
}
