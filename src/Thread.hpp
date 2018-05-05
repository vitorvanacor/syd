#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

class Thread
{
public:
    ~Thread();

    void start();

    virtual void* run() = 0;

    static void* static_run(void *void_this);

private:
    pthread_t id;
};

#endif
