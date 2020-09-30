#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<sys/wait.h>


int main(int argc, char** argv){
    if(argc < 3){
        printf("Please enter the number of children and the number of points\n");
        exit(1);
    }
    
    int fd[2];
    pipe(fd);
    
    const int NUM_CHILD = atoi(argv[1]); // Number of child processes
    const int NUM_POINT = atoi(argv[2]); // Number of sample points
    
    int count = 0;
    for(int i=0; i<NUM_CHILD; i++){
        const int child_points = (i == NUM_CHILD-1) ? NUM_POINT-count : ceil((double) NUM_POINT/NUM_CHILD);
        
        count+=child_points;
        
        pid_t pid = fork();
        
        if(pid<0){
            printf("%s%d\n","Fork Failed at ",i);
            exit(1);
        }
        else if(pid == 0){
            int cpoints = 0; // Number of points inside the circle
            
            for(int i = 0; i<child_points; i++){
                double x = -1 + ((double) rand() / RAND_MAX) * 2;
                double y = -1 + ((double) rand() / RAND_MAX) * 2;
                
                if(sqrt(pow(x,2)+pow(y,2))<=1)
                    ++cpoints;
            }
            
            double pi = (double) 4 * cpoints / child_points;
            FILE* ptr = fdopen(fd[1], "w");
            fprintf(ptr, "%f", (double) i);
            fclose(ptr);
            
            exit(0);
        }
    }
    
    for(int i=0; i<NUM_CHILD; i++){
        pid_t pid = wait(NULL);
        
        double pi;
        FILE* ptr = fdopen(fd[0], "r");
        fscanf(ptr, "%f", pi);
        fclose(ptr);
        
        printf("%s%f%s%f\n", "Child pid ", pid, " approximated pi as ", pi);
    }
        
    return 0;
}
