#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <netdb.h>

#include <string>

using namespace std;

class Socket
{
public:
    Socket(int port);
    ~Socket();

    void bind_server();
    string receive();
    void reply(string msg);

    void set_host(char* hostname);
    void send(string msg);
    string wait_reply();

private:
    int descriptor;
    struct sockaddr_in address;
    struct sockaddr_in sender_address;
    hostent* host;

};

#endif
