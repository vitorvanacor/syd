#include "Socket.hpp"

#include <string.h> // bzero
#include <netdb.h> // hostent
#include <unistd.h> // close
#include <sys/socket.h> //socket

#include <iostream>
#include <stdexcept> //exceptions

#include "sydUtil.h"

using namespace std;

Socket::Socket(int _port)
{
    port = _port;
    id = socket(AF_INET, SOCK_DGRAM, 0);
    debug("Creating Socket " + to_string(id) + " at port " + to_string(port));
    if (id < 0)
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
    debug("Closing socket " + to_string(id));
    close(id);
}

void Socket::bind_server()
{
    debug("Binding server");
    
    address.sin_addr.s_addr = INADDR_ANY;
    if (bind(id, (struct sockaddr *) &address, socklen))
    {
        throw runtime_error("ERROR: Bind failed");
    }
}

void Socket::set_host(string hostname)
{
    debug("Setting host to " + hostname, __FILE__, __LINE__);
    host = gethostbyname(hostname.c_str());
    if (host == NULL)
    {
        throw invalid_argument("ERROR: Invalid hostname");
    }
    address.sin_addr = *((struct in_addr *)host->h_addr);
}

string Socket::receive()
{
    int n = recvfrom(id, receive_buffer, SOCKET_BUFFER_SIZE, 0, (struct sockaddr *) &sender_address, &socklen);
    if (n < 0)
    {
        throw runtime_error("ERROR: Failed to receive message");
    }
    debug("Bytes received: " + to_string(n));
    return string(receive_buffer);
}


void Socket::send(string bytes)
{
    sockaddr_in target_address;
    if (dest_address.sin_port)
    {
        debug("Sending bytes to dest...");
        target_address = dest_address;
    }
    else if (address.sin_addr.s_addr)
    {
        debug("Sending bytes to host...");
        target_address = address;
    }
    else
    {
        debug("ERROR: no host nor destination address set", __FILE__, __LINE__);
        return;
    }
    send_buffer = bytes.c_str();
    int n = sendto(id, send_buffer, strlen(send_buffer), 0,(struct sockaddr *) &target_address, socklen);
    if (n < 0)
    {
        throw runtime_error("ERROR: Failed to send");
    }
    debug("Bytes sent: " + to_string(n));
}

sockaddr_in Socket::get_sender_address()
{
    return sender_address;
}

void Socket::set_dest_address(sockaddr_in new_dest_address)
{
    dest_address = new_dest_address;
}