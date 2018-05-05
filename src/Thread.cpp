#include "Thread.hpp"

Thread::~Thread() {};

void Thread::start()
{
    pthread_create(&id, 0, &Thread::static_run, this);
}

void* Thread::static_run(void *void_this)
{
    Thread* thread_this = static_cast<Thread*>(void_this);
    return thread_this->run();
}
