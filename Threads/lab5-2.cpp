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
    static int index, COUNT;
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
        if(COUNT == 0){
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
    if(argc<3){
        printf("Usage: %s NUM_CUST BUFF_SIZE\n", argv[0]);
        return 1;
    }
    
    const int NUM_CUST = atoi(argv[1]);
    const int BUFF_SIZE = atoi(argv[2]);
    int buffer[BUFF_SIZE];
    QSemaphore cChair(BUFF_SIZE);
    QSemaphore bChair;
    
    Barber barber(buffer, BUFF_SIZE, &cChair, &bChair);
    Customer *customer[NUM_CUST];
    
    barber.start();
    
    for(int i=0; i<NUM_CUST; i++){
        customer[i] = new Customer(buffer, BUFF_SIZE, &cChair, &bChair);
        customer[i]->start();
    }
    
    for(int i=0; i<NUM_CUST; i++){
        customer[i]->wait();
        delete customer[i];
    }
    
    barber.wait();
    
    printf("\nTotal Haircuts = %d\n", barber.getTotal());
    printf("Total No Haircuts = %d\n", NUM_CUST-barber.getTotal());
    
    return 0;
}
