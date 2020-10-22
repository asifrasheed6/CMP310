#include<stdio.h>
#include<stdlib.h>
#include<QThread>
#include<QMutex>
#include<QSemaphore>

class Customer : public QThread{
private:
    int id;
    int *buffer;
    int BUFF_SIZE;
    QSemaphore *cChair, *bChair;
public:
    static int index, COUNT, NUM_BARB;
    static QMutex mutex;
    
    Customer(int *buffer, int BUFF_SIZE, QSemaphore *cChair, QSemaphore *bChair) : id(COUNT++), buffer(buffer), BUFF_SIZE(BUFF_SIZE), cChair(cChair), bChair(bChair) {}
    
    int getID(){
        return id;
    }
    
    void run(){
        if(cChair->tryAcquire()){
            mutex.lock();
            printf("Customer %d sitting in chair index %d\n", id, index);
            buffer[index++] = id;
            index %= BUFF_SIZE;
            mutex.unlock();
            bChair->release();
        }
        else
            printf("Customer %d leaving shop, NO empty chair...\n", id);
    }
    
    ~Customer(){
        --COUNT;
        for (int i=0; COUNT == 0 && i<NUM_BARB; i++){
            cChair->acquire();
            mutex.lock();
            buffer[index++] = -1;
            index %= BUFF_SIZE;
            mutex.unlock();
            bChair->release();
        }
    }
};
int Customer::index = 0;
int Customer::NUM_BARB;
int Customer::COUNT = 0;
QMutex Customer::mutex;

class Barber : public QThread{
private:
    int id, total = 0;
    int *buffer;
    int BUFF_SIZE;
    QSemaphore *cChair, *bChair;
public:
    static int COUNT, index;
    static QMutex mutex;
    
    Barber(int *buffer, int BUFF_SIZE, QSemaphore *cChair, QSemaphore *bChair) : id(COUNT++), buffer(buffer), BUFF_SIZE(BUFF_SIZE), cChair(cChair), bChair(bChair) {}
    
    int getTotal(){
        return total;
    }
    
    void run(){
        while(1){
            bChair->acquire();
            mutex.lock();
            int customer = buffer[index++];
            index %= BUFF_SIZE;
            mutex.unlock();
            cChair->release();
            
            if(customer == -1)
                break;
            
            ++total;
            printf("Barber %d doing haircut for Customer %d\n", id, customer);
            int time = rand() % 5 + 1;
            sleep(time);
            printf("Barber %d finished hairut for customer %d in approx %d seconds\n", id, customer, time);
        }
        printf("===> Barber %d exiting, did haircut for %d customers\n", id, total);
    }
};
int Barber::COUNT = 0;
int Barber::index = 0;
QMutex Barber::mutex;

int main(int argc, char** argv){
    if(argc<4){
        printf("Usage: %s NUM_BARB NUM_CUST BUFF_SIZE\n", argv[0]);
        return 1;
    }
    
    int count = 0;
    const int NUM_BARB = atoi(argv[1]);
    const int NUM_CUST = atoi(argv[2]);
    const int BUFF_SIZE = atoi(argv[3]);
    int buffer[BUFF_SIZE];
    QSemaphore cChair(BUFF_SIZE);
    QSemaphore bChair;
    
    Customer::NUM_BARB = NUM_BARB;
    
    Barber *barber[NUM_BARB];
    Customer *customer[NUM_CUST];
    
    for(int i=0; i<NUM_BARB; i++){
        barber[i] = new Barber(buffer, BUFF_SIZE, &cChair, &bChair);
        barber[i]->start();
    }
    
    for(int i=0; i<NUM_CUST; i++){
        customer[i] = new Customer(buffer, BUFF_SIZE, &cChair, &bChair);
        customer[i]->start();
    }
    
    for(int i=0; i<NUM_CUST; i++){
        customer[i]->wait();
        delete customer[i];
    }
    
    for(int i=0; i<NUM_BARB; i++){
        barber[i]->wait();
        count += barber[i]->getTotal();
    }
    
    printf("\nTotal Haircuts = %d\n", count);
    printf("Total No Haircuts = %d\n", NUM_CUST-count);
        
    return 0;
}
