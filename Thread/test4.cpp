#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define CONSUMER_NUM 4
#define PRODUCER_NUM 1

struct threadParam
{
    int thread_id;
};

pthread_t pids[CONSUMER_NUM + PRODUCER_NUM];
int ready = 0;

pthread_mutex_t mutex;
pthread_cond_t has_product;

void *producer(void* arg)
{
    int no = arg;
    for(; ;)
    {
        pthread_mutex_lock(&mutex);

        ready++;
        printf("producer %d, produce product\n", no);

       pthread_cond_signal(&has_product);
        printf("producer %d, signal\n", no);
        
        pthread_mutex_unlock(&mutex);
        sleep(10);
    }
}

void *consumer(void* arg)
{
    int num=arg;
    printf("%d consumer product\n", num);

    for(; ;)
    {
        pthread_mutex_lock(&mutex);

        while(ready == 0)
       {
           printf("%d consumer wait\n", num);
            pthread_cond_wait(&has_product, &mutex);
        }

        ready--;
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

int main()
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&has_product, NULL);
    printf("init\n");

    for(int i =0; i < PRODUCER_NUM; i++)
    {
        printf("producer %d\n", i);
        struct threadParam param;
        param.thread_id=i;
        pthread_create(&pids[i], NULL, producer, i);
    }
    for(int j = 0; j < CONSUMER_NUM; j++)
   {
        struct threadParam param;
        param.thread_id=j;
        printf("consumer %d\n", param.thread_id);
        int temp_j = j;
        pthread_create(&pids[PRODUCER_NUM+j], NULL, consumer,j );
   }
   for(int k =0; k < CONSUMER_NUM+PRODUCER_NUM; k++)
   {
        pthread_join(pids[k], NULL);
   }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&has_product);
    
}
