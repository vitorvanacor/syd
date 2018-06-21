#ifndef SERVER_H
#define SERVER_H

#include "Util.hpp"

#include "ServerThread.hpp"

class Server
{
  public:
    void start(int port);

    Connection* listener;
    map<string, ServerThread *> threads;

  private:
    void receive_connection(Socket listener);
    void delete_closed_threads();
};

#endif
