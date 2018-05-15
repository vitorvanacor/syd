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
  void send_ack(bool ok = true);
  int send_file(string filepath);
  void send_string(string data);
  void resend();

  Message receive(string expected_type);
  Message receive(list<string> expected_types);
  Message receive_request();
  bool receive_ack();
  int receive_file(string filepath);
  string receive_string();
  string list_server_dir(string dirpath);

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