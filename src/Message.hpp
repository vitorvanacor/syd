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
    OK,
    ERROR,
    LOGIN,
    LIST_SERVER,
    DOWNLOAD,
    UPLOAD,
    BYE,
    END,
    FILE,
    MODTIME,
    SYNC,
    DELETE,
    DONE,
    BACKUP,
    CLIENT_CONNECT,
    CLIENT_DISCONNECT,
    HEARTBEAT
  };

  Message(string session, int sequence, Message::Type type, string content);

  string stringify();
  void print(char direction = '\0');

  static list<Message::Type> type_request();
  static list<Message::Type> type_sync();
  static list<Message::Type> type_action();
  static list<Message::Type> type_file();
  static list<Message::Type> type_backup();
  static Message parse(string msg);

  Message::Type type;
  string session;
  int sequence;
  string content;

  static const string MSG_SEPARATOR;

  static string str(Message::Type t);
};

#endif
