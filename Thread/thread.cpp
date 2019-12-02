#include <pthread.h>
#include <sys/time.h>
#include "thread.h"

static void* threadFunc(void * threadobject)
{
    Thread* thread = (Thread*)threadobject;
    return thread->run(thread->threadFuncParam);
}