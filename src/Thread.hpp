#ifndef THREAD_H
#define THREAD_H

#include "Util.hpp"

class Thread
{
public:
    Thread() : first_write(true) {};
    virtual ~Thread() = 0;

    void start();
    void log(string msg);

    virtual void* run() = 0;

    static void* static_run(void *void_this);

protected:
    string log_file;
    bool first_write;

private:
    pthread_t id;
};

#endif
