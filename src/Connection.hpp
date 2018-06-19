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
  static Connection* listener(int port);

  void connect_to_host(string hostname, int port);
  void connect();
  void confirm();

  bool send(Message::Type type, string content = "");
  void send_ack(bool ok = true);
  void send_file(string filename);
  void send_long_content(Message::Type type, string content = "");
  void resend();

  Message receive(Message::Type expected_type);
  Message receive(list<Message::Type> expected_types);
  Message receive_request();
  bool receive_ack();
  void receive_file(string filepath);
  string receive_content(Message::Type expected_type);
  string receive_long_content(Message::Type expected_type);
  Connection* receive_connection();

  string session;
  Socket *sock;

private:
  void just_send(Message::Type type, string content = "");
  Message just_receive();
  int content_space(Message::Type type);
  void init_sequences();

  map<int, string> messages_sent;
  int last_sequence_sent;
  int last_sequence_received;
};

#endif