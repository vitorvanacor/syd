#ifndef MESSAGE_H
#define MESSAGE_H

#include "sydUtil.h"

#define HEADER_SEPARATOR '|'

class Message
{
public:
  Message(string session, int sequence, string type, string content);

  string to_string();
  void print(char direction = '\0', string username = "");
  bool is_request();

  static Message parse(string msg);
  static Message parseb(char *msg);

  string type;
  string session;
  int sequence;
  string content;

  static const string MSG_SEPARATOR;
  static const string MSG_END;

  static const string T_SYN;
  static const string T_ACK;
  static const string T_LS;
  static const string T_DOWNLOAD;
  static const string T_UPLOAD;
  static const string T_BYE;
  static const string T_FILE;
  static const string T_SOF;
  static const string T_EOF;
  static const string T_ERROR;
  static const string T_SYNC;
  static const string T_STAT;
  static const string T_DONE;
  static const string T_EQUAL;
  static const string T_DEL;
};

#endif
