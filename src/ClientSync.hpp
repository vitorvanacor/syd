#ifndef CLIENTSYNC_H
#define CLIENTSYNC_H

#include "Util.hpp"

#include "Client.hpp"
#include "Thread.hpp"
#include "Connection.hpp"

class ClientSync : public Thread
{
  public:
    ClientSync(Client *client);
    ~ClientSync();
    void *run();

    bool is_open;

  private:
    void sync_own_files();
    void sync_file(File file);
    void receive_files_from_server();

    Connection *connection;
    Client *client;
};

#endif