#ifndef CLIENTSYNC_H
#define CLIENTSYNC_H

#include "Util.hpp"

#include "Thread.hpp"
#include "Socket.hpp"
#include "Connection.hpp"

class ClientSync: public Thread
{
public:
    ClientSync(Connection* connection);
    ~ClientSync();
    void* run();

    bool is_open;

private:
    Connection* connection;
};

#endif