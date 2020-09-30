#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<sys/wait.h>

/*
 Program to calculate pi approximations using Monte Carlo method.
 The pagram accepts number of child processes and number of sample points used for the approximation as command line parameters.
 */

int main(int argc, char** argv){
    // If not all command line parameters are passed
    if(argc < 3){
        printf("Usage: ./monte number_of_children number_of_points\n");
        exit(1);
    }
    
    const int NUM_CHILD = atoi(argv[1]); // Number of child processes
    const int NUM_POINT = atoi(argv[2]); // Number of sample points
    double pi[NUM_CHILD]; // Array to store pi approximations
    double run_time[NUM_CHILD]; // Run time for each child process
    double total_time = 0; // Run time for all the child processes
    
    int count = 0; // Number of sample points assigned to child. Once all children are assigned sample points, count = NUM_POINT
    for(int i=0; i<NUM_CHILD; i++){
        int fd[2];
        pipe(fd);
        
        const int child_points = (i == NUM_CHILD-1) ? NUM_POINT-count : ceil((double) NUM_POINT/NUM_CHILD); // To balance the number of sample points assigned to each child (in case NUM_POINT is not a multiple of NUM_CHILD)
        
        count+=child_points;
        
        pid_t pid = fork();
        
        if(pid<0){
            printf("%s%d\n","Fork Failed at ",i);
            exit(1);
        }
        /*
         Each child will generate x and y coordinates for the number of sample points assigned to it.
         If the distance between the point and origin is less than or equal to 1, it is considered to be inside the circle.
         From the sample points, the child counts the number of points inside the circle and return it to the parent through a pipe.
         */
        else if(pid == 0){
            int cpoints = 0; // Number of points inside the circle
            
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
         Parent points waits for the child to terminate.
         Parent calculates the total wait time for the child process to terminate (which is the total run time for the child).
         Parent adds the total run time of the child to the total run time of all children.
         Once child terminates, the parent reads the number of sample points inside the circle from the pipe.
         Using the number of sample points inside the circle and the total number of points assigned to the child (total number of points assigned to the child = total number of points in a rectangle) calculates the value of pi.
         The value of pi is store in the array pi.
         */
        else{
            time_t start = time(NULL);
            wait(NULL);
            run_time[i] = time(NULL) - start;
            total_time += run_time[i];
            
            char cpoints[256];
            FILE* ptr = fdopen(fd[0], "r");
            fscanf(ptr, "%s", cpoints);
            fclose(ptr);
            
            pi[i] = (double) 4 * atoi(cpoints) / child_points; // Number of points inside the rectangle = child_points
        }
    }
    
    // Prints all the pi approximations
    for(int i=0; i<NUM_CHILD; i++){
        printf("%s%d%s%f\n", "Pi Approximation ", i+1, " = ", pi[i]);
        printf("%s%d%s%f\n", "Speedup time for child process ", i, " = ", run_time[i]/total_time);
    }
    
    return 0;
}
