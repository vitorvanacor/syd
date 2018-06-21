#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "Util.hpp"

#include "ServerSync.hpp"

class Server;

class ServerThread : public Thread
{
  public:
    ServerThread(Server* server, Connection *connection);
    ~ServerThread();
    void *run();

    void mainloop();
    void receive_upload(string filename, Connection* connection = NULL);
    void send_download(string filename, Connection* connection = NULL);
    void list_server();
    void close_session();

    bool is_open;
    string username;
    Connection *connection;
    Server* server;
    ServerSync* server_sync;
    
};

#endif