#ifndef SERVERSYNC_H
#define SERVERSYNC_H

#include "Util.hpp"

#include "ServerThread.hpp"
#include "Socket.hpp"
#include "Connection.hpp"

class ServerSync : public Thread
{
  public:
    ServerSync(ServerThread* server);
    ~ServerSync();
    void *run();

    bool is_open;
    list<string> files_in_transfer;

  private:
    void sync_client_files();
    void send_files_to_client();

    Connection *connection;
    ServerThread* server;
    list<string> updated_files;
};

#endif