#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<QThread>
#include<QMutex>
#include<QMutexLocker>
#include<QWaitCondition>

class Buffer{
private:
    int buffer_size, *buffer, available = 0, in = 0, out = 0;
    QMutex mutex;
    QWaitCondition bufferFull, bufferEmpty;
    
public:
    Buffer(int buffer_size) : buffer_size(buffer_size), buffer(new int[buffer_size]) {}
    
    void insert(int element){
        QMutexLocker locker(&mutex);
        while(available == buffer_size){
            bufferFull.wait(&mutex);
        }
        buffer[in++] = element;
        in %= buffer_size;
        available++;
        bufferEmpty.wakeAll();
    }
    
    int retrieve(){
        QMutexLocker lock(&mutex);
        while(available == 0){
            bufferEmpty.wait(&mutex);
        }
        int ret = buffer[out++];
        out %= buffer_size;
        --available;
        bufferFull.wakeAll();
        return ret;
    }
    
    int getSize(){
        return buffer_size;
    }
    
    ~Buffer(){
        delete []buffer;
    }
};

class Producer : public QThread{
private:
    int id, total;
    Buffer *buffer;
public:
    static int COUNT;
    Producer(int id, int total, Buffer *buffer) : id(id), total(total), buffer(buffer) { ++COUNT; }
    
    void run(){
        srand(time(NULL));
        printf("Producer %d will produce %d random numbers\n", id, total);
        for(int i=0; i<total; i++) buffer->insert(rand());
    }
    
    ~Producer(){
        --COUNT;
        for(int i=0; COUNT==0 && i<buffer->getSize(); i++) buffer->insert(-1);
    }
};
int Producer::COUNT = 0;

class Consumer : public QThread{
private:
    int id, total;
    Buffer* buffer;
public:
    Consumer(int id, Buffer *buffer) : id(id), total(0), buffer(buffer) {}
    
    void run(){
        while(1){
            int temp = buffer->retrieve();
            if(temp == -1) break;
            ++total;
        }
        printf("Consumer %d read a total of %d random numbers\n", id, total);
    }
};

int main(int argc, char** argv){
    if(argc<4){
        printf("Usage: %s NUM_PRODUCER NUM_CONSUMER NUM_NUMBERS\n", argv[0]);
        return 1;
    }
    
    const int NUM_PRODUCERS = atoi(argv[1]);
    const int NUM_CONSUMERS = atoi(argv[2]);
    const int NUM_NUMBERS = atoi(argv[3]);
    
    Buffer buffer(10);
    Producer *producer[NUM_PRODUCERS];
    Consumer *consumer[NUM_CONSUMERS];
    
    int count = 0;
    for(int i=0; i<NUM_PRODUCERS; i++){
        srand(time(NULL));
        int total = i == NUM_PRODUCERS - 1 ? NUM_NUMBERS - count : abs(rand() % (NUM_NUMBERS - count));
        count+=total;
        
        producer[i] = new Producer(i, total, &buffer);
        producer[i]->start();
    }
    
    for(int i=0; i<NUM_CONSUMERS; i++){
        consumer[i] = new Consumer(i, &buffer);
        consumer[i]->start();
    }
    
    for(int i=0; i<NUM_PRODUCERS; i++){
        producer[i]->wait();
        delete producer[i];
    }
    
    for(int i=0; i<NUM_CONSUMERS; i++) consumer[i]->wait();
    
    return 0;
}
