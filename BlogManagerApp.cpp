
// This App retrieves and displays specific posts.

#include <string>
#include <iostream>
#include <pthread.h>
#include <cstdlib>

using namespace std;


void *PrintHello(void *threadid) {
   long tid;
   tid = (long)threadid;
   cout << "Hello World! Thread ID, " << tid << endl;
   
   pthread_exit(NULL);
}



int main ()
{
    std::cout << "\n\nWelcome To Blog Manager!\n\n";
        
   pthread_t clientThread;
   int threadResult;
   int cThreadId=99;
    
    cout << "main() : creating http client thread, " << endl;
    threadResult = pthread_create(&clientThread, NULL, PrintHello, (void *)cThreadId);
         
    if (threadResult){
        cout << "Error:unable to create thread," << threadResult << endl;
        exit(-1);
    }
    
    pthread_exit(NULL);
    
    return 0;
}
