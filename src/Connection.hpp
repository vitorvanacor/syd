#ifndef CONNECTION_H
#define CONNECTION_H

#include "sydUtil.h"

#include "Socket.hpp"
#include "Message.hpp"
#include "File.hpp"

#define FROM_CLIENT 0
#define FROM_SERVER 1

#define TO_CLIENT 0
#define TO_SERVER 1

class Connection
{
  public:
    Connection(string username, string host, int port);
    Connection(string username, string session, Socket *new_socket);
    ~Connection();

    void accept_connection();

    void send(string type, string content = "");
    void sendb(string stype, char *content = NULL);
    void send_ack();
    void send_file(string filename, int to_whom);
    void resend();

    Message receive(string expected_type);
    Message receive_request();
    void receive_ack();
    void receive_file(string filename, int from_who);

    static void *server_thread(void *void_this);

    string username;
    string user_directory;

  private:
    Message receive();
    void init_sequences();

    Socket *sock;
    string session;

    map<int, string> messages_sent;
    int last_sequence_sent;
    int last_sequence_received;
};

#endif