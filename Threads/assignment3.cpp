#include<stdio.h>
#include<string.h>
#include<QThread>
#include<QMutex>

/*
 Program to count the number of times a string pattern is encountered in a text file. Written by Abdullah Siddiqui (00075201) and Asif Rasheed (00073877).
 The program accepts string pattern and the text document subjected to the search as command line parameters.
 The program uses multiple threads for the search. The number of threads are equal to the number of logical cores in the system.
*/

class SearchTask : public QThread{
private:
  int ID; // Thread ID
public:
  static FILE* file; // Input file
  static QMutex mutex; // Mutex for synchronizing count
  static char* pattern; // String pattern to be counted
  static int count; // Number of occurences of the pattern in the text file

  SearchTask(int id) : ID(id){} // Constructor, assigns id

  /*
    Each thread would:
      1. Read the file word by word.
      2. Compares each word with the pattern.
      3. If the pattern is equal to or is a substring of the word (case sensitive), count is increased by one.
  */
  void run(){
      char buffer[256];
      while(fscanf(file, "%s", buffer) != EOF){
        if(strstr(buffer, pattern) != NULL){
          mutex.lock();
          ++count;
          mutex.unlock();
        }
      }
  }

  int getID(){ return ID; } // return ID

  /*
    Checks whether the file has been closed or not.
    If the file is open, closes the file and nullifies the class attribute file.
  */
  ~SearchTask(){
    if(file != NULL && fclose(file) == 0)
      file = NULL;
  }
};
// Declaration for all static class attributes
FILE* SearchTask::file = NULL;
char* SearchTask::pattern;
int SearchTask::count = 0;
QMutex SearchTask::mutex;

int main(int argc, char** argv){
  // Checks if all the command line parameters are given. If any parameter is missing, the program exits with status 1.
  if(argc < 3){
    printf("Usage: %s pattern file\n", argv[0]);
    exit(1);
  }

  // Checks if the path exists or not. If the path doesn't exist, the program exits with status 1.
  FILE* ptr = fopen(argv[2], "r");
  if(ptr == NULL){
    printf("Invalid file!\n");
    exit(1);
  }

  const int NUM_CORE = std::thread::hardware_concurrency(); // Number of logical
  SearchTask::pattern = argv[1]; // Search Pattern
  SearchTask::file = ptr; // File subjected to the search
  SearchTask* threads[NUM_CORE];

  // Creates NUM_CORE threads and starts the search
  for(int i=0; i<NUM_CORE; i++){
    threads[i] = new SearchTask(i);
    threads[i]->start();
  }

  // Waits for all the threads
  for(int i=0; i<NUM_CORE; i++)
    threads[i]->wait();

  // Prints the result and number of logical cores in the system
  printf("Number of logical cores in your system = %d\n", NUM_CORE);
  printf("Number of times %s was encountered in %s = %d\n", argv[1], argv[2], SearchTask::count);

  // Delocates all dynamic memory
  for(int i=0; i<NUM_CORE; i++)
    delete threads[i];

  return 0;
}
