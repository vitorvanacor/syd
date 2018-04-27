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

    socklen = sizeof(struct sockaddr_in);
}

Socket::~Socket()
{
    close(descriptor);
}

void Socket::bind_server()
{
    address.sin_addr.s_addr = INADDR_ANY;
    bzero(&(address.sin_zero), 8);    
    if (bind(descriptor, (struct sockaddr *) &address, socklen))
    {
        throw runtime_error("ERROR: Bind failed");
    }
}

string Socket::receive()
{
    int n = recvfrom(descriptor, receive_buffer, 256, 0, (struct sockaddr *) &their_address, &socklen);
    if (n < 0)
    {
        throw runtime_error("ERROR: Failed to receive message");
    }
    return string(receive_buffer);
}

void Socket::send(string msg)
{
    if (!their_address.sin_port)
    {
        cout << "Tried to send with their_address not set" << endl;
        return;
    }
    send_buffer = msg.c_str();
    int n = sendto(descriptor, send_buffer, strlen(send_buffer), 0,(struct sockaddr *) &their_address, socklen);
    if (n < 0)
    {
        throw runtime_error("ERROR: Failed to send");
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

void Socket::send_to_host(string msg)
{    
    send_buffer = msg.c_str();
    int n = sendto(descriptor, send_buffer, strlen(send_buffer), 0, (const struct sockaddr *) &address, socklen);
    if (n < 0)
    {
        throw runtime_error("ERROR: Failed to send message");
    }
}
