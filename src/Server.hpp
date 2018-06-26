#ifndef SERVER_H
#define SERVER_H

#include "Util.hpp"

#include "ServerThread.hpp"

class Server
{
  public:
    void master_loop(int port);
    void backup_loop(string master_ip, int port);

    Connection* listener;

    int port;
    map<string, ServerThread *> threads;
    list<Connection*> backups;

  private:
    void receive_connection(Socket listener);
    void delete_closed_threads();
};

#endif
