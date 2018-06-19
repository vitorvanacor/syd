#ifndef SERVER_H
#define SERVER_H

#include "Util.hpp"

#include "ServerThread.hpp"

class Server
{
  public:
    void start(int port);

  private:
    void receive_connection(Socket listener);
    void delete_closed_threads();

    map<string, ServerThread *> threads;
};

#endif
