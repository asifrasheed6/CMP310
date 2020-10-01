#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<sys/wait.h>

/*
 Program to approximate the value of pi using Monte Carlo method. Written by Abdullah Siddiqui (00075201) and Asif Rasheed (00073877).
 The pagram accepts number of child processes and number of sample points used for the approximation as command line parameters.
 The program uses multiple pipes for communication between the child processes and the parent process.
*/

int main(int argc, char** argv){
    if(argc < 3){
        printf("Usage: %s children points\n", argv[0]);
        exit(1);
    }
    
    const int NUM_CHILD = atoi(argv[1]); // Number of child processes
    const int NUM_POINT = atoi(argv[2]); // Number of sample points
    int ctotal = 0; // Sum total of the number of sample points inside the circle calculated by each child prrocess
    int fd[NUM_CHILD][2]; // File Descriptor for Pipes
    
    int count = 0; // Number of sample points assigned to the child processes. Once all child processes are assigned sample points, count = NUM_POINT
    for(int i=0; i<NUM_CHILD; i++){
        if(pipe(fd[i]) == -1){
            printf("Failed to create pipe\n");
            exit(1);
        }

        const int child_points = (i == NUM_CHILD-1) ? NUM_POINT-count : ceil((double) NUM_POINT / NUM_CHILD); // To balance the number of sample points assigned to each child (if NUM_POINT is not a multiple of NUM_CHILD)
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
         The child process returns its pid, number of points inside the circle and total number of points assigned to it
        */
        else if(pid == 0){
            int cpoints = 0;
            
            for(int i = 0; i<child_points; i++){
                double x = -1 + ((double) rand() / RAND_MAX) * 2;
                double y = -1 + ((double) rand() / RAND_MAX) * 2;
                
                if(sqrt(pow(x,2)+pow(y,2))<=1)
                    ++cpoints;
            }
            
            FILE* ptr = fdopen(fd[i][1], "w");
            fprintf(ptr, "%d %d %d\n", getpid(), cpoints, child_points);
            fclose(ptr);
            
            exit(0);
        }
    }
    
    // Waits for all the child processes to terminate
    for(int i=0; i<NUM_CHILD; i++)
        wait(NULL);
        
    char cpid[256]; // Process id of the child process
    char cpoints[256]; // Number of points inside the circle out of the total number of sample points assigned to the child process
    char spoints[256]; // Number of sample points assigned to the child process
    
    /*
     Calculates and prints the pi approximation using the number of sample points assigned to the child process and the number of points inside the circle out of the total number of sample points assigned for each child point.
     */
    for(int j=0; j<NUM_CHILD; j++){
        FILE* ptr = fdopen(fd[j][0], "r");
    
        fscanf(ptr, "%s", cpid);
        fscanf(ptr, "%s", cpoints);
        fscanf(ptr, "%s", spoints);

        ctotal += atoi(cpoints);
        printf("The pi approximated by child pid %s = %f\n", cpid, (double) 4 * atoi(cpoints) / atoi(spoints));
    
        fclose(ptr);
    }
    
    // Prints the pi approximatio using the total number of points in side the circle out of all the sample points
    printf("Pi Approximation using number of points from all child processes = %f\n", (double) 4 * ctotal / NUM_POINT);
    return 0;
}
