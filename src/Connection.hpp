#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include<map>

#include "Socket.hpp"
#include "Message.hpp"

using namespace std;

class Connection
{
public:
    Connection(string username, string host, int port);
    Connection(string username, string session, Socket* new_socket);
    ~Connection();

    void accept_connection();

    void send(string type, string content = "");
    void send_ack();
    void resend();

    Message receive();
    Message receive(string expected_type);
    void receive_ack();

    static void* server_thread(void* void_this);


//private:
    void init_sequences();

    Socket* sock;
    string session;
    string username;

    map<int,string> messages_sent;
    int last_sequence_sent;
    int last_sequence_received;
};

#endif