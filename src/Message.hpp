#ifndef MESSAGE_H
#define MESSAGE_H

#include "Util.hpp"

#define HEADER_SEPARATOR '|'

class Message
{
public:
  enum Type
  {
    SYN,
    ACK,
    LOGIN,
    LIST_SERVER,
    DOWNLOAD,
    UPLOAD,
    BYE,
    END,
    FILE,
    MODTIME,
    END_OF_FILE,
    ERROR,
    SYNC,
    STAT,
    DONE,
    EQUAL
  };

  Message(string session, int sequence, Message::Type type, string content);

  string stringify();
  void print(char direction = '\0', string username = "");
  bool is_request();

  static Message parse(string msg);

  Message::Type type;
  string session;
  int sequence;
  string content;

  static const string MSG_SEPARATOR;

  static string str(Message::Type t);
};

#endif
