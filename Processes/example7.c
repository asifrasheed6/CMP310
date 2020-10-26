#include<stdio.h>
#include<unistd.h>

int main(){
    FILE* fh = popen("/usr/sbin/system_profiler SPHardwareDataType | /usr/bin/grep Memory", "r");
    
    char buffer[1024];
    fscanf(fh, "%s %s", buffer, buffer);
    printf("%s\n", buffer);
    close(fh);
}
