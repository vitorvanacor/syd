#include "Message.hpp"

const string Message::MSG_SEPARATOR = "|";

const string Message::T_SYN = "SYN";
const string Message::T_ACK = "ACK";
const string Message::T_LS = "LS";
const string Message::T_DOWNLOAD = "DOWN";
const string Message::T_UPLOAD = "UP";
const string Message::T_BYE = "BYE";
const string Message::T_FILE = "FILE";
const string Message::T_SOF = "SOF";
const string Message::T_EOF = "EOF";
const string Message::T_ERROR = "ERRO";
const string Message::T_SYNC = "SYNC";
const string Message::T_STAT = "STAT";
const string Message::T_DONE = "DONE";
const string Message::T_EQUAL = "EQAL";
const string Message::T_ELECTION = "ELEC";
const string Message::T_ALIVE = "ALIV";
const string Message::T_COORD = "COOR";

Message::Message(string session, int sequence, string type, string content = "")
{
    this->session = session;
    this->sequence = sequence;
    this->type = type;
    this->content = content;
}

string Message::to_string()
{
    string msg = "";
    msg += session;
    msg += Message::MSG_SEPARATOR;
    msg += std::to_string(sequence);
    msg += Message::MSG_SEPARATOR;
    msg += type;
    msg += Message::MSG_SEPARATOR;
    msg += content;
    msg += Message::MSG_SEPARATOR;
    return msg;
}

bool Message::is_request()
{
    return (type == Message::T_LS || type == Message::T_DOWNLOAD || type == Message::T_UPLOAD || type == Message::T_BYE);
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
    debug("|  Sequence |  " + std::to_string(sequence));
    debug("|  Type     |  " + type);
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
    string type = msg.substr(sequence_sep + 1, type_len);
    string content = msg.substr(type_sep + 1, content_len);

    return Message(session, stoi(sequence), type, content);
}
