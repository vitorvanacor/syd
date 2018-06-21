#ifndef SYDCLIENT_H
#define SYDCLIENT_H

#include "Util.hpp"

#include "Connection.hpp"

class Client
{
  public:
    ~Client();
    void start(string username, string hostname, int port);
    void upload_file(string filename, string dirpath = "", Connection* connection = NULL);
    void download_file(string filename, string dirpath = "", Connection* connection = NULL);

    Connection *connection;
    string user_dir;

  private:
    void mainloop();
    
    void list_server();
    void close_session();

};

#endif
