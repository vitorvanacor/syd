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

bool Message::is_request()
{
    switch (type)
    {
    case LIST_SERVER:
    case DOWNLOAD:
    case UPLOAD:
    case BYE:
        return true;
    default:
        return false;
    }
}

void Message::print(char direction, string username)
{
    if (direction == '>')
    {
        debug("===>");
    }
    else if (direction == '<')
    {
        debug("<===");
    }
    debug("|  Session  |  " + session + " " + username);
    debug("|  Sequence |  " + to_string(sequence));
    debug("|  Type     |  " + str(type));
    if (!content.empty())
    {
        debug("|  Content  |  " + content);
    }
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
    case END_OF_FILE:
        return "END_OF_FILE";
    case ERROR:
        return "ERROR";
    case SYNC:
        return "SYNC";
    case STAT:
        return "STAT";
    case DONE:
        return "DONE";
    case EQUAL:
        return "EQUAL";
    }
}
