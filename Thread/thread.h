#ifndef _H_THREAD
#define _H_THREAD

class Thread
{
    public:
        Thread();
        virtual ~Thread();
        int start(void* = NULL);
        void stop();
        void sleep(int);
        void detach();
        void *wait();
    private:
        pthread_t handle;
        bool started;
        bool detached;
        void* threadFuncParam;
   friend void* threadFunc(void*);
};

static void *threadFunc(void*);

#endif