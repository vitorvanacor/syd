#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <netdb.h>

#include <string>

using namespace std;

#define SOCKET_BUFFER_SIZE 100
#define PACKET_SIZE 10

class Socket
{
  public:
    Socket(int port);
    ~Socket();

    void bind_server();
    void set_host(string hostname);

    void send(string msg);
    void sendb(char *content);
    string receive();

    void set_timeout(int seconds);
    sockaddr_in get_sender_address();
    void set_dest_address(sockaddr_in new_dest_address);

  private:
    int id;
    int port;
    int total_send;
    int total_received;
    struct sockaddr_in server_address;
    struct sockaddr_in sender_address;
    struct sockaddr_in dest_address;
    struct hostent *host;
    char receive_buffer[SOCKET_BUFFER_SIZE];
    const char *send_buffer;

    struct timeval timeout;
    socklen_t socklen;
};

#endif
