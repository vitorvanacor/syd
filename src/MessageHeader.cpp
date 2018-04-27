#include "MessageHeader.hpp"

#include "sydUtil.h"

using namespace std;

MessageHeader::MessageHeader(string type, string session, string sequence)
{
    this->type = type;
    this->session = session;
    this->sequence = sequence;
}

MessageHeader::~MessageHeader() {};

string MessageHeader::to_string()
{
    int i;
    string header = type;
    for (i=type.length(); i<TYPE_LEN; i++)
    {
        header += HEADER_FILLER;
    }
    header += HEADER_SEPARATOR;
    header += session;
    for (i=session.length(); i<SESSION_LEN; i++)
    {
        header += HEADER_FILLER;
    }
    header += HEADER_SEPARATOR;
    header += sequence;
    for (i=sequence.length(); i<SEQUENCE_LEN; i++)
    {
        header += HEADER_FILLER;
    }
    header += HEADER_SEPARATOR;
    return header;
}

void MessageHeader::print()
{
    debug("");
    debug("  Type     |  " + type);
    debug("  Session  |  " + session);
    debug("  Sequence |  " + sequence);
    debug("");
}

MessageHeader MessageHeader::parse(string msg)
{
    string type = msg.substr(0, TYPE_LEN);
    string session = msg.substr(TYPE_LEN+1, SESSION_LEN);
    string sequence = msg.substr(TYPE_LEN+SESSION_LEN+2, SEQUENCE_LEN);
    return MessageHeader(type, session, sequence);
}
