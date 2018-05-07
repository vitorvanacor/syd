#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

using namespace std;

#define HEADER_SEPARATOR '|'

class Message
{
public:
    Message(string session, int sequence, string type, string content);

    string to_string();
    void print(char direction = '\0', string username = "");
    bool is_request();

    static Message parse(string msg);

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
};

#endif
