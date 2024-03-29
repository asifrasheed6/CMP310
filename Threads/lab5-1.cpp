#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<QThread>
#include<QMutex>
#include<QSemaphore>

#define BUFFER_SIZE 10

/*
 Producer/Consumer Problem that creates M producer threads and N consumer threads. The producer threads will generate NUM random numbers.
 M, N and NUM are passed as commandline parameters.
 */
class Consumer : public QThread{
private:
    int id, *buffer, total = 0;
    QSemaphore *space, *nitems;
    
public:
    static QMutex mutex;
    static int out, COUNT;
    
    Consumer(int *buffer, QSemaphore *space, QSemaphore *nitems) : id(COUNT++), buffer(buffer), space(space), nitems(nitems) {}
    
    static int getCount(){
        return COUNT;
    }
    
    void run(){
        int val;
        while(1){
            nitems->acquire();
            
            mutex.lock();
            val = buffer[out++];
            out%=BUFFER_SIZE;
            mutex.unlock();
            
            space->release();
            
            if(val == -1) break;
            else ++total;
        }
        printf("Consumer %d read a total of %d random numbers\n", id, total);
    }
};
QMutex Consumer::mutex;
int Consumer::out = 0;
int Consumer::COUNT = 0;

class Producer : public QThread{
private:
    int id, total, *buffer;
    QSemaphore *space, *nitems;

public:
    static QMutex mutex;
    static int in, COUNT;
    
    Producer(int total, int *buffer, QSemaphore *space, QSemaphore* nitems) : id(COUNT++), total(total), buffer(buffer), space(space), nitems(nitems){}
    
    void run(){
        printf("Producer %d will produce %d random numbers.\n", id, total);
        
        srand(time(NULL));
        
        for(int i=0; i<total; i++){
            space->acquire();
            
            mutex.lock();
            buffer[in++] = rand();
            in%=BUFFER_SIZE;
            mutex.unlock();
            
            nitems->release();
        }
    }
    
    ~Producer(){
        --COUNT;
        for(int i=0; COUNT == 0 && i < Consumer::getCount(); i++){
            space->acquire();
            
            mutex.lock();
            buffer[in++] = -1;
            in%=BUFFER_SIZE;
            mutex.unlock();
            
            nitems->release();
        }
    }
};

QMutex Producer::mutex;
int Producer::in = 0;
int Producer::COUNT = 0;

int main(int argc, char** argv){
    if(argc<4){
        printf("Usage: %s NUM_PRODUCER NUM_CONSUMER NUM_NUMBERS\n", argv[0]);
        exit(1);
    }
    
    const int NUM_PRODUCERS = atoi(argv[1]);
    const int NUM_CONSUMERS = atoi(argv[2]);
    const int NUM_NUMBERS = atoi(argv[3]);
        
    int buffer[BUFFER_SIZE];
    QSemaphore space(BUFFER_SIZE);
    QSemaphore nitems;
    int count = 0;
    
    Producer* producer[NUM_PRODUCERS];
    Consumer* consumer[NUM_CONSUMERS];
    
    for(int i=0; i<NUM_PRODUCERS; i++){
        srand(time(NULL));
        int total = i == NUM_PRODUCERS-1 ? NUM_NUMBERS - count : abs(rand() % (NUM_NUMBERS - count));
        count += total;
        
        producer[i] = new Producer(total, buffer, &space, &nitems);
        producer[i]->start();
    }
    
    for(int i=0; i<NUM_CONSUMERS; i++){
        consumer[i] = new Consumer(buffer, &space, &nitems);
        consumer[i]->start();
    }
    
    for(int i=0; i<NUM_PRODUCERS; i++){
        producer[i]->wait();
        delete producer[i];
    }
    
    for(int i=0; i<NUM_CONSUMERS; i++)
        consumer[i]->wait();
    
    return 0;
}
