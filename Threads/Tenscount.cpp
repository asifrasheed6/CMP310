#include <QThread>
#include <stdio.h>

using namespace std;

class TensCount : public QThread{
private:
    int *data;
    int N;
public:
    int localRes=0;
    TensCount(int *d, int n):data(d), N(n){}
    void run()
    {
       
        for(int i=0;i<N;i++)
          {
              
              if(data[i] !=0 && data[i]%10 == 0)
                localRes++;
          }
        
    }
};

int main(int argc, char *argv[])
{
    int M = atoi(argv[1]);
 


    int data[] = {9 , 10 , 10 , 0, 3, 220, 11, 1};
    int N = sizeof(data)/sizeof(data[0]);
    TensCount *t[M];
    int perThr = (N + M -1)/M; //(int)ceil
    int start = 0;

    for(int i = 0; i < M; i++ )
    {
        int toProcess = (i == M-1) ? (N - start) : perThr;
        t[i]=new TensCount (data+start, toProcess);
        t[i]->start();
        start+=perThr;
    }

    int total = 0;
    for(int i = 0; i < M; i++ )
    {
        t[i]->wait();
        total+=t[i]->localRes;
       }

       printf("%d\n", total);
    return 0;

}
