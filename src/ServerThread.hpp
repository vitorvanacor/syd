#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "Util.hpp"

#include "Thread.hpp"
#include "Socket.hpp"
#include "Connection.hpp"

class ServerThread : public Thread
{
  public:
    ServerThread(Connection *connection);
    ~ServerThread();
    void *run();

    void mainloop();
    void receive_file(string filename);
    void send_file(string filename);
    void list_server();
    void close_session();

    bool is_open;

  private:
    Connection *connection;
    string username;
};

#endif