#ifndef CONNECTION_H
#define CONNECTION_H

#include "sydUtil.h"

#include "Socket.hpp"
#include "Message.hpp"
#include "File.hpp"

class Connection
{
public:
  Connection(string username, string session = "", Socket *new_socket = NULL);
  ~Connection();

  void connect();
  void accept_connection();

  void send(string type, string content = "");
  void sendb(string stype, char *content = NULL);
  void send_ack();
  int send_file(string filepath);
  void resend();

  Message receive(string expected_type, string other_type = " ");
  Message receive_request();
  void receive_ack();
  int receive_file(string filepath);

  static void *server_thread(void *void_this);

  string username;
  string user_directory;
  string session;
  Socket *sock;

private:
  Message receive();
  void init_sequences();

  map<int, string> messages_sent;
  int last_sequence_sent;
  int last_sequence_received;
};

#endif