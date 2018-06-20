#ifndef SOCKET_H
#define SOCKET_H
#include "Util.hpp"

#define SOCKET_BUFFER_SIZE 2000

class Socket
{
  public:
    Socket(int port);
    ~Socket();

    void bind_server();
    void set_host(string hostname);

    void send(string msg);
    string receive();

    void set_timeout(int seconds);
    void enable_timeout();
    void disable_timeout();

    Socket* get_answerer();
    void set_to_answer(Socket* sock);

    static int DEFAULT_TIMEOUT;

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
    struct timeval zero_timeout;
    socklen_t socklen;
};

#endif
