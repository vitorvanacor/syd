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

    void set_host(char* hostname);
    void send_to_host(string msg);
    void send(string msg);
    string wait_reply();

private:
    int descriptor;
    struct sockaddr_in address;
    struct sockaddr_in their_address;
    hostent* host;
    char receive_buffer[256];
    const char* send_buffer;

    socklen_t socklen;

};

#endif
