#include<stdio.h>
#include<string.h>
#include<fstream>
#include<QThread>
#include<QMutex>

/*
 Program to count the number of times a string pattern is encountered in a text file.
 The program accepts string pattern and path to the text document subjected to the search as command line parameters.
 The program uses multiple threads for the search.
 The number of threads are equal to the number of logical cores in the system.
 Written by Abdullah Siddiqui (00075201) and Asif Rasheed (00073877).
*/

class SearchTask : public QThread{
private:
  int ID; // Thread ID
  
  /*
  Thread safe method to read from Input File Stream.
  Synchronizes Input File Stream using Mutex.
  Returns false if at the end of file. Else true.
  */
  bool readFromFile(char* buffer){
    mutex[0].lock();
    bool ret = file.peek() != EOF;
    file.getline(buffer, 1024, '\n');
    mutex[0].unlock();
    return ret;
  }
 
public:
  static std::ifstream file; // Input file stream
  static QMutex mutex[2]; // Mutexes for synchronizing count and file stream
  static char* pattern; // String pattern to be searched
  static int count; // Number of occurences of the pattern in the text file

  SearchTask(int id) : ID(id){} // Constructor, assigns id

  /*
  1. Each thread reads a line from the given file.
  2. Ignores all the characters from the line until the first occurence of the string pattern.
  3. If the resulting subbuffer is equal to NULL (that is if the string pattern is not in the subbuffer), read the next line and repeat 2 to 7.
  4. Else increment count synchronously.
  5. Ignores the first strlen(pattern) charaters from the subbuffer.
  6. Ignores all the characters from the subbuffer until the next occurence of the pattern.
  7. Repeats 3, 4, 5 and 6.
  */
  void run(){
    char buffer[1024];
    while(readFromFile(buffer)){
      char* subbuffer = strstr(buffer, pattern);
      while(subbuffer != NULL){
        mutex[1].lock();
        ++count;
        mutex[1].unlock();
        subbuffer = strstr(&subbuffer[strlen(pattern)], pattern);
      }
    }
  }

  int getID(){ return ID; } // returns ID

  // Closes the input file stream
  ~SearchTask(){
    file.close();
  }
};
// Declaration for all static class attributes
std::ifstream SearchTask::file;
char* SearchTask::pattern;
int SearchTask::count = 0;
QMutex SearchTask::mutex[2];

int main(int argc, char** argv){
  // Checks if all the command line parameters are given. If any parameter is missing, the program exits with status 1.
  if(argc < 3){
    printf("Usage: %s pattern file\n", argv[0]);
    exit(1);
  }

  const int NUM_CORE = std::thread::hardware_concurrency(); // Number of logical cores in the system
  SearchTask::pattern = argv[1]; // Search Pattern
  SearchTask::file.open(argv[2]); // Tries to open the target file path

  // If the file stream fails, the program exists with status 1.
  if(SearchTask::file.fail()){
    printf("Invalid File!\n");
    exit(1);
  }

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
