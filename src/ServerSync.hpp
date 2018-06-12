#ifndef SERVERSYNC_H
#define SERVERSYNC_H

#include "sydUtil.h"

#include "ServerThread.hpp"
#include "Thread.hpp"
#include "Socket.hpp"
#include "Connection.hpp"

class ServerSync : public Thread
{
  public:
    ServerSync(Connection *connection, map<string, ServerThread *> sync_threads);
    ~ServerSync();
    void *run();

    bool is_open;
    map<string, ServerThread *> sync_threads;
    list<string> files_in_transfer;

  private:
    Connection *connection;
};

#endif