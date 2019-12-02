#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
using namespace std;

pthread_mutex_t mutex;
void *PrintHello(void* threadid)
{
    int tid;
    tid = *(int*)threadid;
    for(int i = 0; i < 5; i++)
    {
        cout << "tid" << tid << ",i=" << i << endl;
         sleep(1);
    }
    cout << "hello World Thread Id, " << tid << endl;
    //pthread_exit(NULL);
}

int main()
{
    pthread_t threads[5];
    int rc;
    int i;
    for(i =0; i < 5; i++)
    {
        threads[i] = i;
        cout << "main(): creating thread, " << i << endl;
        rc = pthread_create(&threads[i], NULL, PrintHello, &i);
        if(rc)
        {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
        //pthread_join(threads[i], NULL);
        //pthread_detach(threads[i]);
        sleep(1);
    }

    //显示调用线程退出
  // pthread_exit(NULL);
   while(1);
   // return 0;
}
