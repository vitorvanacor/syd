#ifndef MESSAGEHEADER_H
#define MESSAGEHEADER_H

#include <string>

using namespace std;

#define TYPE_LEN 8
#define SESSION_LEN 4
#define SEQUENCE_LEN 4
#define HEADER_LEN (TYPE_LEN + SESSION_LEN + SEQUENCE_LEN + 3)
#define HEADER_SEPARATOR '|'
#define HEADER_FILLER ' '

class MessageHeader
{
public:
    MessageHeader(string type, string session, string sequence);
    ~MessageHeader();

    string to_string();
    void print();

    static MessageHeader parse(string msg);

    string type;
    string session;
    string sequence;

};

#endif
