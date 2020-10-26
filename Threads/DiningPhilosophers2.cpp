#include<stdio.h>
#include<QThread>
#include<QMutex>
#include<QSemaphore>

/*
 Dining Philosophers Problem Approach 2: Only 4 philosopers eat at the same time
 */

class Philisopher : public QThread{
private:
    int id;
public:
    static QSemaphore chopsticks[5];
    static QSemaphore seats;
    
    Philisopher(int id) : id(id) {}
    
    void run(){
        seats.acquire();
        chopsticks[id].acquire();
        chopsticks[(id+1) % 5].acquire();
        printf("Philosopher %d is eating...\n", id);
        sleep(rand()%5);
        printf("Philosopher %d is done eating\n", id);
        chopsticks[id].release();
        chopsticks[(id+1) % 5].release();
        seats.release();
    }
};
QSemaphore Philisopher::chopsticks[5];
QSemaphore Philisopher::seats(4);

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
