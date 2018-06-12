#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "sydUtil.h"

#include "Thread.hpp"
#include "Socket.hpp"
#include "Connection.hpp"

class ServerThread : public Thread
{
public:
  ServerThread(Connection *connection, map<string, ServerThread *> threads, map<string, ServerThread *> sync_threads);
  ~ServerThread();
  void *run();

  bool is_open;
  map<string, ServerThread *> threads;
  map<string, ServerThread *> sync_threads;

private:
  Connection *connection;
};

#endif