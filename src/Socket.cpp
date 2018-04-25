#include "Socket.h"

#include <string.h> // bzero
#include <netdb.h> // hostent
#include <unistd.h> // close
#include <sys/socket.h> //socket

#include <iostream>
#include <stdexcept> //exceptions

using namespace std;

Socket::Socket(int port)
{
    descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (descriptor < 0)
    {
        throw runtime_error("ERROR: Socket creation failed");
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    bzero(&(address.sin_zero), 8);
}

Socket::~Socket()
{
    close(descriptor);
}

void Socket::bind_server()
{
    address.sin_addr.s_addr = INADDR_ANY;
    bzero(&(address.sin_zero), 8);    

    if (bind(descriptor, (struct sockaddr *) &address, sizeof(struct sockaddr)) < 0)
    {
        throw runtime_error("ERROR: Bind failed");
    }
}

string Socket::receive()
{
    char buffer[256];
    socklen_t socklen = sizeof(struct sockaddr_in);
    int n = recvfrom(descriptor, buffer, 256, 0, (struct sockaddr *) &sender_address, &socklen);
    if (n < 0)
    {
        throw runtime_error("ERROR: Failed to receive message");
    }
    return string(buffer);
}

void Socket::reply(string msg)
{
    if (!sender_address.sin_port)
    {
        cout << "Reply only possible after receive" << endl;
        return;
    }
    const char* buffer = msg.c_str();
    int n = sendto(descriptor, buffer, strlen(buffer), 0,(struct sockaddr *) &sender_address, sizeof(struct sockaddr));
    if (n < 0)
    {
        throw runtime_error("ERROR: Failed to reply");
    }
}

void Socket::set_host(char* hostname)
{
    host = gethostbyname(hostname);
    if (host == NULL)
    {
        throw invalid_argument("ERROR: Invalid hostname");
    }
    address.sin_addr = *((struct in_addr *)host->h_addr);
}

void Socket::send(string msg)
{
    const char* buffer = msg.c_str();
    int n = sendto(descriptor, buffer, strlen(buffer), 0, (const struct sockaddr *) &address, sizeof(struct sockaddr_in));
    if (n < 0)
    {
        throw runtime_error("ERROR: Failed to send message");
    }
}

string Socket::wait_reply()
{
    char buffer[256];
    sockaddr_in server_address;
    unsigned int socklen = sizeof(struct sockaddr_in);
    int n = recvfrom(descriptor, buffer, 256, 0, (struct sockaddr *) &server_address, &socklen);
    if (n < 0)
    {
        throw runtime_error("ERROR: Receive message failed");
    }
    return string(buffer);
}
