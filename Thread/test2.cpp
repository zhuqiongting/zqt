#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int i =0;
pthread_mutex_t mutex;

void *thr_fun(void *arg)
{
    pthread_mutex_lock(&mutex);
    char* no = (char*)arg;
    for(; i < 5; i++)
    {
        printf("%s thread, i:%d\n", no, i);
        sleep(2);
    }
    i = 0;
    pthread_mutex_unlock(&mutex);
}

int main()
{
    pthread_t tid1, tid2;
    pthread_mutex_init(&mutex, NULL);

    char buf1[10] = "No1";
    char buf2[10] = "No2";
    pthread_create(&tid1, NULL, thr_fun, buf1);
    pthread_create(&tid2, NULL, thr_fun, buf2);

    sleep(1);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    pthread_mutex_destroy(&mutex);

    while(1);
    //return 0;
}

