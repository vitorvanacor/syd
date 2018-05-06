#include "Socket.hpp"

#include <string.h> // bzero
#include <netdb.h> // hostent
#include <unistd.h> // close
#include <sys/socket.h> //socket

#include <iostream> //cout
#include <stdexcept> //exceptions

#include "sydUtil.h"

using namespace std;

Socket::Socket(int _port)
{
    port = _port;
    id = socket(AF_INET, SOCK_DGRAM, 0);
    debug("Creating Socket " + to_string(id) + " at port " + to_string(port), __FILE__);
    if (id < 0)
    {
        throw runtime_error("ERROR: Socket creation failed");
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    bzero(&(server_address.sin_zero), 8);

    socklen = sizeof(struct sockaddr_in);
    host = NULL;

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);
}

Socket::~Socket()
{
    debug("Closing socket " + to_string(id));
    close(id);
}

void Socket::bind_server()
{
    debug("Binding server", __FILE__);
    server_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(id, (struct sockaddr *) &server_address, socklen))
    {
        throw runtime_error("ERROR: Bind failed");
    }
}

void Socket::set_host(string hostname)
{
    debug("Setting host to " + hostname, __FILE__);
    host = gethostbyname(hostname.c_str());
    if (host == NULL)
    {
        throw invalid_argument("ERROR: Invalid hostname");
    }
    server_address.sin_addr = *((struct in_addr *)host->h_addr);
}

string Socket::receive()
{
    memset(receive_buffer, 0, sizeof(receive_buffer));
    int n = recvfrom(id, receive_buffer, SOCKET_BUFFER_SIZE, 0, (struct sockaddr *) &sender_address, &socklen);
    if (n < 0)
    {
        if (errno == EWOULDBLOCK)
        {
            throw timeout_exception();
        }
        throw runtime_error("ERROR: Failed to receive message");
    }
    debug("Bytes received: " + to_string(n), __FILE__);
    return string(receive_buffer);
}


void Socket::send(string bytes)
{
    struct sockaddr_in* target_address;
    if (dest_address.sin_port)
    {
        target_address = &dest_address;
    }
    else if (host)
    {
        debug("Target: "+string(host->h_name));
        target_address = &server_address;
    }
    else
    {
        debug("ERROR: no host nor destination address set", __FILE__, __LINE__);
        return;
    }

    debug("Sending "+to_string(bytes.length())+" bytes...", __FILE__);
    send_buffer = bytes.c_str();
    int n = sendto(id, send_buffer, strlen(send_buffer), 0,(const struct sockaddr *) target_address, socklen);
    if (n < 0)
    {
        throw runtime_error("ERROR: Failed to send");
    }
    debug("Bytes sent: " + to_string(n), __FILE__);
}

void Socket::set_timeout(int seconds)
{
    timeout.tv_sec = seconds;
    setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);
}

sockaddr_in Socket::get_sender_address()
{
    return sender_address;
}

void Socket::set_dest_address(sockaddr_in new_dest_address)
{
    dest_address = new_dest_address;
}