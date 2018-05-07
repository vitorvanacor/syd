#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <map>

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
    void send_file(char* fileByteArray, int fileLength);
    void resend();

    Message receive(string expected_type);
    Message receive_request();
    void receive_ack();
    void receive_file();

    static void* server_thread(void* void_this);

    string username;

private:
    Message receive();
    void init_sequences();

    Socket* sock;
    string session;

    map<int,string> messages_sent;
    int last_sequence_sent;
    int last_sequence_received;
};

#endif