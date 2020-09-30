#include<stdio.h>

int main(){
    int N;
    FILE* ptr = popen("cat /proc/cpuinfo | grep processor | wc", "r");
    fscanf(ptr, "%i", &N);
    printf("%s%i%s\n","Your computer has ",N," CPU Cores");
    return 0;
}
