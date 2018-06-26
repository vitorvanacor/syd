#ifndef SERVER_H
#define SERVER_H

#include "Util.hpp"

#include "ServerThread.hpp"

class Server
{
  public:
    Server(int port);
    void start();
    void backup(string hostname);

    Connection* listener;
    Socket* sock;
    int port;
    map<string, ServerThread *> threads;
    static map<string, Socket> backups;
    static list<string> client_ips;

  private:
    void receive_connection(Socket listener);
    void delete_closed_threads();
};

#endif
