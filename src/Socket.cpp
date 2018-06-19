#include "Socket.hpp"

int Socket::DEFAULT_TIMEOUT = 1;

Socket::Socket(int _port)
{
    port = _port;
    id = socket(AF_INET, SOCK_DGRAM, 0);
    debug("Creating Socket " + to_string(id) + " at port " + to_string(port), __FILE__);
    if (id < 0)
    {
        throw runtime_error(strerror(errno));
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    bzero(&(server_address.sin_zero), 8);
    host = NULL;

    // Used by sendto and rcvfrom
    socklen = sizeof(struct sockaddr_in);

    // Set timeout
    timeout.tv_sec = DEFAULT_TIMEOUT;
    timeout.tv_usec = 0;
    setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout);
    zero_timeout.tv_sec = 0;
    timeout.tv_usec = 0;
}

Socket::~Socket()
{
    debug("Closing socket " + to_string(id));
    close(id);
}

void Socket::bind_server()
{
    debug("Binding server", __FILE__);
    set_timeout(0); // Never timeout
    server_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(id, (struct sockaddr *)&server_address, socklen))
    {
        throw runtime_error(strerror(errno));
    }
}

void Socket::set_host(string hostname)
{
    debug("Setting host to " + hostname, __FILE__);
    host = gethostbyname(hostname.c_str());
    if (host == NULL)
    {
        throw invalid_argument(strerror(errno));
    }
    server_address.sin_addr = *((struct in_addr *)host->h_addr);
}

string Socket::receive()
{
    memset(receive_buffer, 0, sizeof(receive_buffer));
    int n = recvfrom(id, receive_buffer, SOCKET_BUFFER_SIZE, 0, (struct sockaddr *)&sender_address, &socklen);
    if (n < 0)
    {
        if (errno == EWOULDBLOCK)
        {
            throw timeout_exception();
        }
        throw runtime_error(strerror(errno));
    }
    debug("Bytes received: " + to_string(n), __FILE__);
    return string(receive_buffer, n);
}

void Socket::send(string bytes)
{
    struct sockaddr_in *target_address;
    if (dest_address.sin_port)
    {
        target_address = &dest_address;
    }
    else if (host)
    {
        debug("Target: " + string(host->h_name));
        target_address = &server_address;
    }
    else
    {
        debug(to_string(id)+": ERROR: no host nor destination address set", __FILE__, __LINE__);
        return;
    }

    debug("Sending " + to_string(bytes.length()) + " bytes...", __FILE__);
    send_buffer = bytes.data();
    int n = sendto(id, send_buffer, bytes.length(), 0, (const struct sockaddr *)target_address, socklen);
    if (n < 0)
    {
        perror("ERROR in socket send: ");
    }
    debug("Bytes sent: " + to_string(n), __FILE__);
}

void Socket::set_timeout(int seconds)
{
    timeout.tv_sec = seconds;
    setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout);
}

void Socket::enable_timeout()
{
    setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout);
}

void Socket::disable_timeout()
{
    setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, (const char *)&zero_timeout, sizeof timeout);
}

Socket *Socket::get_answerer()
{
    Socket *answerer = new Socket(port);
    answerer->set_to_answer(this);
    return answerer;
}

void Socket::set_to_answer(Socket* sock)
{
    dest_address = sock->sender_address;
}
