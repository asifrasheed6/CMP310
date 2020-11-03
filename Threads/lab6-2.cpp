#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<QThread>
#include<QMutex>
#include<QMutexLocker>
#include<QWaitCondition>

class Seats{
private:
    int buffer_size, *buffer, available = 0, in = 0, out = 0;
    QMutex mutex;
    QWaitCondition bufferFull, bufferEmpty;
    
public:
    Seats(int buffer_size) : buffer_size(buffer_size), buffer(new int[buffer_size]) {}
    
    void insert(int id){
        QMutexLocker locker(&mutex);
        
        if(id!=-1 && isAllSeatsTaken()){
            printf("Customer %d is leaving the store, NO empty chair...\n", id);
            return;
        }
        else if(isAllSeatsTaken()){
            bufferFull.wait(&mutex);
        }
        
        if(id!=-1) printf("Customer %d sitting in chair index %d\n", id, in);
        
        buffer[in++] = id;
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
    
    bool isAllSeatsTaken(){
        return available == buffer_size;
    }
    
    int getSize(){
        return buffer_size;
    }
    
    ~Seats(){
        delete []buffer;
    }
};

class Barber : public QThread{
private:
    int id, total = 0;
    Seats *seats;
public:
    Barber(int id, Seats *seats) : id(id), seats(seats) {}
    
    void run(){
        while(1){
            int customer = seats->retrieve();
            if(customer == -1) break;
            
            printf("Barber %d doing haircut for customer %d\n", id, customer);
            int time = rand() % 5 + 1;
            sleep(time);
            printf("Barber %d finished haircut for customer %d in approx %d seconds\n", id, customer, time);
            ++total;
        }
        printf("===> Barber %d exiting, did haircut for %d customers\n", id, total);
    }
    
    int getTotalCustomers(){
        return total;
    }
};

class Customer : public QThread{
private:
    int id;
    Seats *seats;
public:
    static int COUNT;
    Customer(int id, Seats *seats) : id(id), seats(seats) { ++COUNT; }
    
    void run(){
        seats->insert(id);
    }
    
    ~Customer(){
        --COUNT;
        for(int i=0; COUNT==0 && i<seats->getSize(); i++) seats->insert(-1);
    }
};
int Customer::COUNT = 0;

int main(int argc, char** argv){
    if(argc<4){
        printf("Usage: %s NUM_BARB NUM_CUST BUFF_SIZE\n", argv[0]);
        return 1;
    }
    
    int count = 0;
    const int NUM_BARB = atoi(argv[1]);
    const int NUM_CUST = atoi(argv[2]);
    const int BUFF_SIZE = atoi(argv[3]);
    
    if(NUM_BARB > BUFF_SIZE){
        printf("The buffer size should be greater than or equal to the number of barbers\n");
        return 1;
    }
    
    Seats seats(BUFF_SIZE);
    Barber *barber[NUM_BARB];
    Customer *customer[NUM_CUST];
    
    for(int i=0; i<NUM_BARB; i++){
        barber[i] = new Barber(i, &seats);
        barber[i]->start();
    }
    
    for(int i=0; i<NUM_CUST; i++){
        customer[i] = new Customer(i, &seats);
        customer[i]->start();
    }
    
    for(int i=0; i<NUM_CUST; i++){
        customer[i]->wait();
        delete customer[i];
    }
    
    for(int i=0; i<NUM_BARB; i++){
        barber[i]->wait();
        count+=barber[i]->getTotalCustomers();
    }
    
    printf("\nTotal Haircurts = %d\n", count);
    printf("\nTotal No Haircuts = %d\n", NUM_CUST-count);
    
    return 0;
}
