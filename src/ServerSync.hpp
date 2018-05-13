#ifndef SERVERSYNC_H
#define SERVERSYNC_H

#include "sydUtil.h"

#include "Thread.hpp"
#include "Socket.hpp"
#include "Connection.hpp"

class ServerSync: public Thread
{
public:
    ServerSync(Connection* connection);
    ~ServerSync();
    void* run();

    bool is_open;

private:
    Connection* connection;
};

#endif