#ifndef CONNECTION_H
#define CONNECTION_H

#include "Util.hpp"

#include "Socket.hpp"
#include "Message.hpp"
#include "File.hpp"

class Connection
{
public:
  Connection(string session = "", Socket *new_socket = NULL);
  ~Connection();

  void connect_to_host(string hostname, int port);
  void connect();
  void accept_connection();

  bool send(Message::Type type, string content = "");
  void send_ack(bool ok = true);
  void send_file(string filepath);
  void send_long_content(Message::Type type, string content = "");
  void resend();

  Message receive(Message::Type expected_type);
  Message receive(list<string> expected_types);
  Message receive_request();
  bool receive_ack();
  void receive_file(string filepath);
  string receive_content(Message::Type expected_type);
  string receive_long_content(Message::Type expected_type);

  string session;
  Socket *sock;

private:
  void just_send(Message::Type type, string content = "");
  Message just_receive();
  void init_sequences();

  map<int, string> messages_sent;
  int last_sequence_sent;
  int last_sequence_received;
};

#endif