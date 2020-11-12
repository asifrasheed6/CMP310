#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<QMutex>
#include<QThread>
#include<QSemaphore>
#include<QMutexLocker>
#include<QWaitCondition>

/*
 Program to solve dining philosophers getting fat problem using monitor.
 A philosopher has 2 neighbor, if a philosopher has dinned more times than one of this neighbor and that neighbour is waiting to get forks, then he will wait until his neighbour has done eating.
 Written by Abdullah Siddiqui (00075201) and Asif Rasheed (00073877).
 */

class ForkTray{
    /*
        This is the monitor class;
        forks is an array of semaphores where each element represent a fork. The number of forks is equal to the number of philosophers.
        num_meals is an array of integer where each element represent the number of meals taken by a philosopher. The number of elements is equal to the number of philosophers.
        lock2 is used to synchronize the use of the array num_meals.
        lock is used to synchronize forks.
     */
private:
    QSemaphore *forks;
    QMutex lock, lock2;
    QWaitCondition waitCondition;
    int num_forks, *num_meals;
public:
    ForkTray(int num_forks) : num_forks(num_forks) {
        forks = new QSemaphore[num_forks];
        num_meals = new int[num_forks];
        for(int i=0; i<num_forks; i++){
            num_meals[i] = 0;
            forks[i].release();
        }
    }
    
    ~ForkTray(){
        delete []forks;
        delete []num_meals;
    }
    
    // The philosopher will only acquire forks if both left and right forks are available else the philosopher need to wait until both becomes available.
    void getForks(int id){
        QMutexLocker locker(&lock);
        while(!forks[id].available() || !forks[(id+1)%num_forks].available()) waitCondition.wait(&lock);
        
        forks[id].acquire();
        forks[(id+1)%num_forks].acquire();
    }
    
    // Releases the fork acquired by the given philosopher and increses the number of meals taken by the philosopher by 1.
    void releaseForks(int id){
        QMutexLocker locker(&lock2);
        forks[id].release();
        forks[(id+1)%num_forks].release();
        num_meals[id]++;
        waitCondition.wakeAll();
    }
    
    // Returns the number of meals taken by the given philosopher
    int getNumMeals(int id){
        QMutexLocker locker(&lock2);
        return num_meals[id];
    }
    
    // Returns the number of meals taken by the first neighbor (the philosopher on the left) of the given philosopher
    int getNumMealsN1(int id){
        QMutexLocker locker(&lock2);
        return num_meals[id-1<0 ? num_forks-1 : id-1];
    }
    
    // Returns the number of melas taken the by the second neighbor (the philosopher on the right) of the given philosopher
    int getNumMealsN2(int id){
        QMutexLocker locker(&lock2);
        return num_meals[(id+1) % num_forks];
    }
};

class Philosopher : public QThread{
private:
    int id, num_meals; // num_meals represent the maximum number of meals that can be taken by a philosopher (not to be confused with num_meals in ForkTray)
    ForkTray *forks;
public:
    Philosopher(int id, int num_meals, ForkTray *forks) : id(id), num_meals(num_meals), forks(forks){}
    
    void eat(){
        srand(time(NULL));
        sleep(rand()%1);
        printf("Philosopher %d ate dinner #%d\n", id, forks->getNumMeals(id)+1);
    }
    
    void think(){
        srand(time(NULL));
        sleep(rand()%1);
    }
    
    void run(){
        think();
        while(1){ // The loop is terminated if the number of meals taken by the philosopher is equal to num_meals
            int meals = forks->getNumMeals(id);
            if(meals==num_meals) break;
            
            // The philosopher will only take the meal if the number of meals taken is less than or equal to the number of meals taken by the neighbors. That is if the philosopher has dinned more times than one of the neighbor, he/she keeps thinking
            if(forks->getNumMealsN1(id) < meals || forks->getNumMealsN2(id) < meals) think();
            else{
                forks->getForks(id);
                printf("Philosopher %d got forks\n", id);
                eat();
                printf("Philosopher %d released the forks\n", id);
                forks->releaseForks(id);
            }
        }
    }
};

int main(int argc, char** argv){
    if(argc < 3){
        printf("Usage: %s philosophers meals\n", argv[0]);
        return 1;
    }
    const int NUM_PHIL = atoi(argv[1]);
    const int NUM_MEAL = atoi(argv[2]);
    ForkTray forks(NUM_PHIL);
    Philosopher *philosopher[NUM_PHIL];

    for(int i=0; i<NUM_PHIL; i++){
        philosopher[i] = new Philosopher(i, NUM_MEAL, &forks);
    }
    
    for(int i=0; i<NUM_PHIL; i++){
        philosopher[i]->start();
    }

    for(int i=0; i<NUM_PHIL; i++){
        philosopher[i]->wait();
        delete philosopher[i];
    }
    
    return 0;
}
