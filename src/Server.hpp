#ifndef SERVER_H
#define SERVER_H

#include "Util.hpp"

#include "ServerThread.hpp"

class Server
{
public:
    void master_loop(int port, list<string> client_ips = {});
    void backup_loop(string master_ip, int port);

    Connection *listener;

    int port;
    map<string, ServerThread *> threads;
    list<Connection *> backups;

private:
    string election();
    void notify_clients(list<string> client_ips);
    void delete_closed_threads();
};

#endif
