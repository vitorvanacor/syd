#ifndef THREAD_H
#define THREAD_H

#include "sydUtil.h"

class Thread
{
public:
    virtual ~Thread() = 0;

    void start();

    virtual void* run() = 0;

    static void* static_run(void *void_this);

private:
    pthread_t id;
};

#endif
