#ifndef SYDCLIENT_H
#define SYDCLIENT_H

#include "Util.hpp"

#include "Thread.hpp"
#include "Connection.hpp"

class Client : public Thread
{
  public:
    Client(string username, string hostname, int port);
    ~Client();
    void *run();
    
    void upload_file(string filename, string dirpath = "", Connection* connection = NULL);
    void download_file(string filename, string dirpath = "", Connection* connection = NULL);

    string username;
    string hostname;
    int port;
    Connection *connection;
    string user_dir;

  private:
    void mainloop();
    
    void list_server();
    void close_session();

};

#endif
