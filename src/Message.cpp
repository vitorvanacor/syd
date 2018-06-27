#include "Message.hpp"

const string Message::MSG_SEPARATOR = "|";

Message::Message(string session, int sequence, Message::Type type, string content = "")
{
    this->session = session;
    this->sequence = sequence;
    this->type = type;
    this->content = content;
}

string Message::stringify()
{
    string msg = "";
    msg += session;
    msg += Message::MSG_SEPARATOR;
    msg += to_string(sequence);
    msg += Message::MSG_SEPARATOR;
    msg += to_string(static_cast<int>(type));
    msg += Message::MSG_SEPARATOR;
    msg += content;
    msg += Message::MSG_SEPARATOR;
    return msg;
}

list<Message::Type> Message::type_request()
{
    list<Message::Type> req;
    req.push_back(Message::Type::LIST_SERVER);
    req.push_back(Message::Type::DOWNLOAD);
    req.push_back(Message::Type::UPLOAD);
    req.push_back(Message::Type::BYE);
    return req;
}

list<Message::Type> Message::type_sync()
{
    list<Message::Type> syn;
    syn.push_back(Message::Type::SYNC);
    syn.push_back(Message::Type::DONE);
    syn.push_back(Message::Type::DELETE);
    return syn;
}

list<Message::Type> Message::type_action()
{
    list<Message::Type> act;
    act.push_back(Message::Type::UPLOAD);
    act.push_back(Message::Type::DOWNLOAD);
    act.push_back(Message::Type::DONE);
    return act;
}

list<Message::Type> Message::type_file()
{
    list<Message::Type> fil;
    fil.push_back(Message::Type::FILE);
    fil.push_back(Message::Type::END);
    return fil;
}

list<Message::Type> Message::type_backup()
{
    list<Message::Type> bac;
    bac.push_back(Message::Type::UPLOAD);
    bac.push_back(Message::Type::DELETE);
    bac.push_back(Message::Type::CLIENT_CONNECT);
    bac.push_back(Message::Type::CLIENT_DISCONNECT);
    bac.push_back(Message::Type::LOGIN);
    return bac;
}

void Message::print(char direction)
{
    string msg;
    if (direction == '>')
    {
        msg += "|===>  ";
    }
    else if (direction == '<')
    {
        msg += "<===|  ";
    }
    msg += "(" + session + ") " + to_string(sequence) + ": " + str(type) + " ";
    if (type == Message::Type::FILE || type == Message::Type::LIST_SERVER)
    {
        msg += "(" + to_string(content.length()) + " bytes)";
    }
    else if (type == Message::Type::MODTIME)
    {
        msg += "(" + time_to_string(stoi(content)) + ")";
    }
    else if (!content.empty())
    {
        msg += content;
    }
    debug(msg, __FILE__);
}

Message Message::parse(string msg)
{
    // Find position of separators
    int session_sep = msg.find('|');
    int sequence_sep = msg.find('|', session_sep + 1);
    int type_sep = msg.find('|', sequence_sep + 1);
    int content_sep = msg.find_last_of('|');

    // Calculate sections length
    int session_len = session_sep;
    int sequence_len = sequence_sep - session_sep - 1;
    int type_len = type_sep - sequence_sep - 1;
    int content_len = content_sep - type_sep - 1;

    // Assign
    string session = msg.substr(0, session_len);
    string sequence = msg.substr(session_sep + 1, sequence_len);
    Message::Type type = static_cast<Message::Type>(stoi(msg.substr(sequence_sep + 1, type_len)));
    string content = msg.substr(type_sep + 1, content_len);

    return Message(session, stoi(sequence), type, content);
}

string Message::str(Message::Type type)
{
    switch (type)
    {
    case SYN:
        return "SYN";
    case ACK:
        return "ACK";
    case ERROR:
        return "ERROR";
    case OK:
        return "OK";
    case LOGIN:
        return "LOGIN";
    case LIST_SERVER:
        return "LIST_SERVER";
    case DOWNLOAD:
        return "DOWNLOAD";
    case UPLOAD:
        return "UPLOAD";
    case BYE:
        return "BYE";
    case FILE:
        return "FILE";
    case MODTIME:
        return "MODTIME";
    case SYNC:
        return "SYNC";
    case DELETE:
        return "DELETE";
    case END:
        return "END";
    case DONE:
        return "DONE";
    case BACKUP:
        return "BACKUP";
    case CLIENT_CONNECT:
        return "CLIENT_CONNECT";
    case CLIENT_DISCONNECT:
        return "CLIENT_DISCONNECT";
    }
    return "INVALID_MESSAGE_TYPE";
}
