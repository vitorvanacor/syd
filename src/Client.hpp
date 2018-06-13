#ifndef SYDCLIENT_H
#define SYDCLIENT_H

#include "Util.hpp"

class Client
{
  public:
    ~Client();
    void start(string username, string hostname, int port);

  private:
    void mainloop();
    void upload_file(string filename);
    void download_file(string filename);
    void list_server();
    void list_client();
    void close_session();

    void filelist_parser(string filelist);

    Connection *connection;
    string user_directory;
}

#endif
