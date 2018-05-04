#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "Thread.hpp"
#include "Socket.hpp"
#include "Connection.hpp"

class ServerThread: public Thread
{
public:
    ServerThread(string username, string session, Socket* new_socket);
    void* run();

private:
    Connection* connection;
};

#endif