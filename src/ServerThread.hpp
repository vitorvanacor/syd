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
    void receive_file(string filename, Connection* connection = NULL);
    void send_file(string filename, Connection* connection = NULL);
    void list_server();
    void close_session();

    bool is_open;
    string username;
    Connection *connection;
    
};

#endif