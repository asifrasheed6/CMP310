#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/time.h>

/*
 Program to calculate pi approximations using Monte Carlo method. Written by Abdullah Siddiqui (00075201) and Asif Rasheed (00073877).
 The pagram accepts number of child processes and number of sample points used for the approximation as command line parameters.
 */

int main(int argc, char** argv){
    // If not all command line parameters are passed
    if(argc < 3){
        printf("Usage: ./monte children points\n");
        exit(1);
    }
    
    const int NUM_CHILD = atoi(argv[1]); // Number of child processes
    const int NUM_POINT = atoi(argv[2]); // Number of sample points
    int ctotal = 0; // Sum of number of points inside the circle calculated by each child
    double pi[NUM_CHILD]; // Array to store pi approximations for each child
    double runtime[NUM_CHILD]; // Array to store run time of each child in milliseconds
    double totaltime = 0; // Total runtime of all the child processes
    
    int count = 0; // Number of sample points assigned to child processes. Once all child processes are assigned sample points, count = NUM_POINT
    for(int i=0; i<NUM_CHILD; i++){
        int fd[2];
        
        // If pipe fails the program prints fail message and terminates with status 1
        if(pipe(fd) == -1){
            printf("Failed to create pipe!\n");
            exit(1);
        }
        
        const int child_points = (i == NUM_CHILD-1) ? NUM_POINT-count : ceil((double) NUM_POINT/NUM_CHILD); // To balance the number of sample points assigned to each child (if NUM_POINT is not a multiple of NUM_CHILD)
        
        count += child_points;
        
        pid_t pid = fork();
        
        if(pid<0){
            printf("%s%d\n","Fork Failed at ",i);
            exit(1);
        }
        /*
         Each child will generate x and y coordinates for the number of sample points assigned to it.
         If the distance between the point and origin is less than or equal to 1, it is considered to be inside the circle.
         From the sample points, the child counts the number of points inside the circle (stored in cpoints) and return it to the parent through a pipe.
         */
        else if(pid == 0){
            int cpoints = 0;
            
            for(int i = 0; i<child_points; i++){
                double x = -1 + ((double) rand() / RAND_MAX) * 2;
                double y = -1 + ((double) rand() / RAND_MAX) * 2;
                
                if(sqrt(pow(x,2)+pow(y,2))<=1)
                    ++cpoints;
            }
            
            FILE* ptr = fdopen(fd[1], "w");
            fprintf(ptr, "%d\n", cpoints);
            fclose(ptr);
            
            exit(0);
        }
        /*
         Parent waits for child to terminate.
         Parent calculates the time elapsed while waiting for the child (which is approximately the runtime of the child process as the time is calculated with respect to the Epoch) and stores it in runtime.
         Parent adds the run time of the child process to totaltime.
         Once child terminates, the parent reads the number of sample points inside the circle from the pipe.
         Using the number of sample points inside the circle and the total number of points assigned to the child (total number of points assigned to the child = total number of points in a rectangle) calculates the value of pi.
         The value of pi is store in the array pi.
         The number of sample points in the circle is added to ctotal.
         */
        else{
            struct timespec child_start_time, child_end_time;
            
            clock_gettime(CLOCK_REALTIME, &child_start_time);
            wait(NULL);
            clock_gettime(CLOCK_REALTIME, &child_end_time);
            
            runtime[i] = (child_end_time.tv_nsec - child_start_time.tv_nsec) * 1e-9;
            totaltime += runtime[i];
                        
            char cpoints[256];
            FILE* ptr = fdopen(fd[0], "r");
            fscanf(ptr, "%s", cpoints);
            fclose(ptr);
            
            ctotal += atoi(cpoints);
            pi[i] = (double) 4 * atoi(cpoints) / child_points;
        }
    }
    
    // Prints all the pi approximations and speedup time for each child process
    for(int i=0; i<NUM_CHILD; i++){
        printf("%s%d%s%f\n", "Pi Approximation ", i+1, " = ", pi[i]);
        printf("%s%d%s%f\n\n", "Speed up time for child ", i+1, " = ", runtime[i]/totaltime);
    }
    
    printf("%s%f\n","Pi Approximation using number of points from all child processes = ", (double) 4 * ctotal / NUM_POINT);
    
    return 0;
}
