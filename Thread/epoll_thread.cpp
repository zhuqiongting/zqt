#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include <error.h>

#define MAXLINE 10
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 8006
#define INFTIM 1000

//线程池任务结构体
struct task
{
    int fd;         //需要读写的文件描述符
    struct task* next;
};

//用于读写两个方面传递参数
struct user_data
{
    int fd;
    unsigned int n_size;
    char line[MAXLINE];
};

//声明epoll_event结构体的变量，ev用于注册事件，数组用于回传要处理的事件
struct epoll_event ev,events[20];
int epfd;
pthread_mutex_t mutex;
pthread_cond_t cond1;
struct task* readhead = NULL, *readtail = NULL, *writehead = NULL;


static int count111 = 0;
static time_t oldtime = 0, nowtime = 0;
//线程的任务函数
void *readtask(void* args)
{
    int fd = -1;
    unsigned int n;

    struct user_data *data = NULL;
    while(1)
    {
        pthread_mutex_lock(&mutex);

        //等待任务队列不为空
        while(readhead == NULL)
            pthread_cond_wait(&cond1, &mutex);

        fd = readhead->fd;

        //从任务队列取出一个读任务
        struct task* tmp = readhead;
        readhead = readhead->next;
        delete tmp;
        pthread_mutex_unlock(&mutex);
        
        data = new user_data();
        data->fd = fd;
        char recvBuf[1024] = {0};
        int ret = 999;
        int rs = 1;
        while(rs)
        {
            ret = recv(fd, recvBuf, 1024, 0);
            if(ret < 0)
            {
                if(errno == EAGAIN)
                {
                    printf("EAGAIN\n");
                    break;
                }
                else
                {
                    printf("recv error\n");
                    close(fd);
                    break;
                }
            }
            else if(ret == 0)
            {
                //表示对端socket正常关闭
                rs = 0;
            }
            if(ret == sizeof(recvBuf))
                rs = 1;     //需要再次读取
            else
                rs = 0;
        }
        //处理接受的数据
        if(ret > 0)
        {
            data->n_size = n;
            count111++;  
        }
           
    }
}

//void *writetask(void *args);



void setnonblocking(int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);
    if(opts < 0)
    {
        perror("fcntl(sock, GETFL)");
        exit(1);
    }
    opts = opts | O_NONBLOCK;
    if(fcntl(sock, F_SETFL, opts) < 0)
    {
        perror("fcntl(sock, SETFL, opts)");
        exit(1);
    }
}

int main()
{
    int i, maxi, listenfd, connfd, sockfd, nfds;
    pthread_t tid1, tid2;

    struct task* new_task = NULL;
    struct user_data* rdata = NULL;
    socklen_t clilen;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond1, NULL);

     //初始化读写线程
     pthread_create(&tid1, NULL, readtask, NULL);
     pthread_create(&tid2, NULL, readtask, NULL);
     
    //生成用于处理accpet的epoll专用文件描述符
    epfd = epoll_create(256);

    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    //吧socket设置为非阻塞方式
    setnonblocking(listenfd);

    //设置要处理的事件相关文件描述符
    ev.data.fd = listenfd;
    //设置要处理的事件类型
    ev.events = EPOLLIN | EPOLLET;
    //注册epoll事件
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERV_PORT);
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    bind(listenfd, (sockaddr*)&serveraddr, sizeof(serveraddr));
    listen(listenfd, LISTENQ);

    maxi = 0;
    for(; ;)
    {
        //等待epoll事件发生
        nfds = epoll_wait(epfd, events, 20, 500);

        //处理所发生的的事件
        for(i =0; i < nfds; i++)
        {
            if(events[i].data.fd == listenfd)
            {
                //有新连接事件
                connfd = accept(listenfd, (sockaddr*)&clientaddr, &clilen);
                if(connfd < 0)
                {
                    perror("connfd < 0");
                    exit(1);
                }

                //处理连接

                setnonblocking(connfd);

                char* str = inet_ntoa(clientaddr.sin_addr);

                ev.data.fd = connfd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
            }
            else if(events[i].events & EPOLLIN)
            {
                //有读事件，接受消息
                if((sockfd = events[i].data.fd) <0) continue;
                new_task = new task();
                new_task->fd = sockfd;
                new_task->next = NULL;

                //添加新任务
                pthread_mutex_lock(&mutex);
                if(readhead == NULL)
                {
                    readhead = new_task;
                    readtail = new_task;
                }
                else
                {
                    readtail->next = NULL;
                    readtail = new_task;
                }

        
                //唤醒所等待cond1条件的线程
                pthread_cond_broadcast(&cond1);
                pthread_mutex_unlock(&mutex);
            }
            else if(events[i].events & EPOLLOUT)
            {
                //有写事件，发生消息

                
            }
        }
    }
    
}


