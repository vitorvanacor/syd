#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "sydUtil.h"

#include "Thread.hpp"
#include "Socket.hpp"
#include "Connection.hpp"

class ServerThread: public Thread
{
public:
    ServerThread(Connection* connection);
    ~ServerThread();
    void* run();

    bool is_open;

private:
    Connection* connection;
};

#endif