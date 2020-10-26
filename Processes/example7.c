#include<stdio.h>
#include<unistd.h>

int main(){
    FILE* fh = popen("system_profiler SPHardwareDataType | grep Memory | wc", "r");
    int N;
    fscanf(fh, "%d", &N);
    printf("%d\n", N);
    fclose(fh);
}
