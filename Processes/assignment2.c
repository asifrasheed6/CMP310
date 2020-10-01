#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/time.h>

/*
 Program to approximate the value of pi using Monte Carlo method. Written by Abdullah Siddiqui (00075201) and Asif Rasheed (00073877).
 The pagram accepts number of child processes and number of sample points used for the approximation as command line parameters.
 The program uses pipes for communication between the child processes and the parent process.
 The program uses time based on the UNIX Epoch to calculate the speedof time for each child process.
 */

int main(int argc, char** argv){
    if(argc < 3){
        printf("Usage: ./monte children points\n");
        exit(1);
    }
    
    int fd[2];
    if(pipe(fd) == -1){
        printf("Failed to create pipe\n");
        exit(1);
    }
    
    const int NUM_CHILD = atoi(argv[1]); // Number of child processes
    const int NUM_POINT = atoi(argv[2]); // Number of sample points
    int ctotal = 0; // Sum total of the number of sample points inside the circle calculated by each child prrocess
    double total_runtime; // Approximate total runtime of the program
    
    struct timespec start_time, end_time; // Used to approximate the runtime of the program
    clock_gettime(CLOCK_REALTIME, &start_time);
    
    int count = 0; // Number of sample points assigned to the child processes. Once all child processes are assigned sample points, count = NUM_POINT
    for(int i=0; i<NUM_CHILD; i++){
        const int child_points = (i == NUM_CHILD-1) ? NUM_POINT-count : ceil((double) NUM_POINT/NUM_CHILD); // To balance the number of sample points assigned to each child (if NUM_POINT is not a multiple of NUM_CHILD)
        count += child_points;
        
        pid_t pid = fork();
        
        if(pid<0){
            printf("Fork Failed for Child %d\n", i+1);
            exit(1);
        }
        /*
         Each child process will generate x and y coordinates for the number of sample points assigned to it.
         If the distance between the point and origin is less than or equal to 1, it is considered to be inside the circle.
         From the sample points, the child process will count the number of points inside the circle (stored in cpoints).
         The child process returns its pid, number of points inside the circle, total number of points assigned to it and the total runtime (in nano seconds) to the parent using pipe.
        */
        else if(pid == 0){
            struct timespec child_start_time, child_end_time;
            int cpoints = 0;
            
            clock_gettime(CLOCK_REALTIME, &child_start_time);
            for(int i = 0; i<child_points; i++){
                double x = -1 + ((double) rand() / RAND_MAX) * 2;
                double y = -1 + ((double) rand() / RAND_MAX) * 2;
                
                if(sqrt(pow(x,2)+pow(y,2))<=1)
                    ++cpoints;
            }
            clock_gettime(CLOCK_REALTIME, &child_end_time);
            
            FILE* ptr = fdopen(fd[1], "w");
            fprintf(ptr, "%d %d %d %d\n", getpid(), cpoints, child_points, abs(end_time.tv_nsec - start_time.tv_nsec));
            fclose(ptr);
            
            exit(0);
        }
    }
    
    // Waits for all the child processes to terminate
    for(int i=0; i<NUM_CHILD; i++)
        wait(NULL);
    
    clock_gettime(CLOCK_REALTIME, &end_time);
    total_runtime = abs(end_time.tv_nsec - start_time.tv_nsec) * 1e-9;
    
    char cpid[256]; // Process id of the child process
    char cpoints[256]; // Number of points inside the circle out of the total number of sample points assigned to the child process
    char spoints[256]; // Number of sample points assigned to the child process
    char runtime[256]; // Total runtime of the child process in nanoseconds
    
    FILE* ptr = fdopen(fd[0], "r");
    /*
     Calculates and prints the pi approximation using the number of sample points assigned to the child process and the number of points inside the circle out of the total number of sample points assigned for each child point.
     Converts the runtime of the child process from nanoseconds to seconds, calculates and prints the speedup time for each child process.
     */
    for(int i=0; i<NUM_CHILD; i++){
        fscanf(ptr, "%s", cpid);
        fscanf(ptr, "%s", cpoints);
        fscanf(ptr, "%s", spoints);
        fscanf(ptr, "%s", runtime);
        
        ctotal += atoi(cpoints);
        printf("The pi approximated by child pid %s = %f\n", cpid, (double) 4 * atoi(cpoints) / atoi(spoints));
        printf("The speedup time for child pid %s = %f\n\n", cpid, (double) (atoi(runtime) * 1e-9) / total_runtime);
    }
    
    fclose(ptr);
    
    // Prints the pi approximatio using the total number of points in side the circle out of all the sample points.
    printf("Pi Approximation using number of points from all child processes = %f\n", (double) 4 * ctotal / NUM_POINT);
    return 0;
}
