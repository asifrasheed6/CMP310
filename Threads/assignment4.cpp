#include<stdio.h>
#include<stdlib.h>
#include<QThread>
#include<QMutex>
#include<QSemaphore>

/*
 Program to solve the Jurassic Park Problem.
 The program accpets the number of visitors and number of cars as command line parameters.
 The program runs multiple visitor and car threads.
 Number of visitor threads are equal to the number of visitors and the number of car threads are equal to the number of cars.
 Each visitor thread and car thread would have reference to a buffer from which cars retrieve waiting passenger, reference to the QSemaphore cars which represents the available cars and reference to the QSemaphore passengers which represents the passangers wating for a car.
 Written by Abdullah Siddiqui (00075201) and Asif Rasheed (00073877).
 */
class Visitor : public QThread{
private:
    Visitor **buffer;
    int id, buffer_size;
    QSemaphore *cars, *passengers;
    
public:
    /*
     The static attributes:
        index represents the next index of the buffer to write to.
        COUNT represents the number of Visitor objects.
        mutex used to synchronize writing to the buffer.
     */
    static int index, COUNT;
    static QMutex mutex;
    
    Visitor(Visitor **buffer, int buffer_size, QSemaphore *cars, QSemaphore *passengers) : buffer(buffer), id(COUNT++), buffer_size(buffer_size), cars(cars), passengers(passengers) {}
    
    int getID(){
        return id;
    }
    
    /*
     Each visitor would sleep for upto 5 seconds.
     Each visitor would wait if no cars are available and once a car is available will write to the buffer.
     It would acquire the QSemaphore cars decreasing the number of available cars and release the QSemaphore passengers increasing the number of passengers.
     */
    void run(){
        sleep(rand() % 6);
        printf("Visitor %d is waiting for a car\n", id);
        cars->acquire();
        mutex.lock();
        buffer[index++] = this;
        index %= buffer_size;
        mutex.unlock();
        passengers->release();
    }
    
    /*
     Destructor is called by a Car thread once "the car drops off the passenger".
     It would decrement COUNT by 1. Once count is zero, would write NULL to all buffer indexes.
     */
    ~Visitor(){
        --COUNT;
        printf("Visitor %d left the park\n", id);
        
        for(int i=0; COUNT == 0 && i<buffer_size; i++){
            cars->acquire();
            mutex.lock();
            buffer[index++] = NULL;
            index %= buffer_size;
            mutex.unlock();
            passengers->release();
        }
    }
};
int Visitor::index = 0;
int Visitor::COUNT = 0;
QMutex Visitor::mutex;

class Car : public QThread{
private:
    Visitor **buffer;
    int id, buffer_size;
    QSemaphore *cars, *passengers;
    
public:
    /*
     The static attributes:
        index represents the next index of the buffer to read from.
        COUNT represents the number of Car objects.
        mutex used to synchronize reading from the buffer.
     */
    static int index, COUNT;
    static QMutex mutex;
    
    Car(Visitor **buffer, int buffer_size, QSemaphore *cars, QSemaphore *passengers) : buffer(buffer), id(COUNT++), buffer_size(buffer_size), cars(cars), passengers(passengers) {}
    
    /*
     If there are no passengers available, the threads would wait. Once there is a visitor available, it would acquire the passenger reducing the number of available passengers, read the visitor from buffer and sleeps for upto 3 seconds. Once woken up, it would release the semaphore cars increasing the number of available cars and deallocate the visitor (drops off the passenger).
     */
    void run(){
        while(1){
            passengers->acquire();
            mutex.lock();
            Visitor *visitor = buffer[index++];
            index %= buffer_size;
            mutex.unlock();
            
            if(visitor == NULL)
                break;
            
            printf("Car %d got visitor %d as a passenger\n", id, visitor->getID());
            sleep(rand() % 4);
            printf("Car %d returned\n", id);
            cars->release();
            delete visitor;
        }
    }
};
int Car::index = 0;
int Car::COUNT = 0;
QMutex Car::mutex;

int main(int argc, char** argv){
    if(argc < 3){
        printf("Usage: %s NUM_VISITORS NUM_CARS\n", argv[0]);
        return 1;
    }
    
    const int NUM_VISITORS = atoi(argv[1]);
    const int NUM_CARS = atoi(argv[2]);
    
    Visitor *buffer[NUM_CARS];
    QSemaphore cars(NUM_CARS), passengers;
    
    Visitor *visitor[NUM_VISITORS];
    Car *car[NUM_CARS];
    
    for(int i=0; i<NUM_CARS; i++){
        car[i] = new Car(buffer, NUM_CARS, &cars, &passengers);
        car[i]->start();
    }
    
    for(int i=0; i<NUM_VISITORS; i++){
        visitor[i] = new Visitor(buffer, NUM_CARS, &cars, &passengers);
        visitor[i]->start();
    }
    
    // Doesn't need to wait for the Visitor threads as all the visitor threads are deallocated by the Car threads.
    for(int i=0; i<NUM_CARS; i++){
        car[i]->wait();
        delete car[i];
    }
    
    return 0;
}
