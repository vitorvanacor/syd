#ifndef SERVERSYNC_H
#define SERVERSYNC_H

#include "Util.hpp"

#include "Thread.hpp"
#include "Connection.hpp"

class ServerThread;

class ServerSync : public Thread
{
  public:
    ServerSync(ServerThread* parent);
    ~ServerSync();
    void *run();

    bool is_open;
    list<string> files_in_transfer;

  private:
    void sync_client_files();
    void sync_file(string filename);
    void delete_file(string filename);
    void send_files_to_client();

    Connection *connection;
    ServerThread* parent;
    list<string> files_not_synced;
    list<string> files_to_delete;
};

#endif