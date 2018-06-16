#ifndef SERVERSYNC_H
#define SERVERSYNC_H

#include "sydUtil.h"
#include "Thread.hpp"
#include "Socket.hpp"
#include "Connection.hpp"

class ServerSync : public Thread
{
  public:
    ServerSync(Connection *connection, map<string, ServerSync *> *sync_threads_pointer);
    ~ServerSync();
    void *run();

    bool is_open;
    map<string, ServerSync *> *sync_threads;
    list<string> files_in_transfer;

  private:
    Connection *connection;
};

#endif