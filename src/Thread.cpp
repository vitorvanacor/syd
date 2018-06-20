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

void Thread::log(string msg)
{
    ofstream out;
    if (first_write)
    {
        out.open(log_file, ios::trunc);
        first_write = false;
    }
    else
    {
        out.open(log_file, ios::app);
    }
    out << time_to_string(time(NULL)) << ": " << msg << endl;
}
