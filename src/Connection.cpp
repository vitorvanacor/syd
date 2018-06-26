#include "Connection.hpp"

Connection::Connection()
{
    session = to_string(rand() % 10000);
    last_sequence_sent = -1;
    last_sequence_received = -1;
}

Connection::Connection(string hostname, int port) : Connection()
{
    sock = new Socket(port);
    sock->set_host(hostname);
    connect();
}

Connection::Connection(int port)
{
    sock = new Socket(port);
    sock->bind_server();
}

Connection::Connection(string session, Socket* sock) : Connection()
{
    this->session = session;
    this->sock = sock;
}

Connection::~Connection()
{
    debug("Closing connection " + session);
    delete this->sock;
}

Connection* Connection::create_connection()
{
    Connection* new_connection = new Connection();
    new_connection->sock = sock->get_answerer();
    new_connection->connect();
    return new_connection;
}

Connection *Connection::receive_connection()
{
    sock->disable_timeout();
    while (true)
    {
        string msg_s = sock->receive();
        Message msg = Message::parse(msg_s);
        msg.print('<');
        if (msg.type == Message::Type::SYN && msg.session != session)
        {
            sock->enable_timeout();
            // if new connection isn't a backup
            if (Server::backups.count(msg.content) <= 0)
            {
                send(Message::Type::NEW_USER, msg.content);
            }
            return new Connection(msg.session, sock->get_answerer());
        }
    }
}

void Connection::connect()
{
    send(Message::Type::SYN);
    sock->set_to_answer(sock);
}

void Connection::confirm()
{
    last_sequence_received = 0;
    send_ack();
}

void Connection::confirm_receipt(Message msg)
{
    last_sequence_received = msg.sequence;
    send_ack();
}

void Connection::just_send(Message::Type type, string content)
{
    last_sequence_sent += 1;
    Message msg = Message(session, last_sequence_sent, type, content);
    msg.print('>');
    sock->send(msg.stringify());
    messages_sent[last_sequence_sent] = msg.stringify();
}

void Connection::send(Message::Type type, string content)
{
    just_send(type, content);
    receive_ack();
}

void Connection::send_long_content(Message::Type type, string content)
{
    string buffer;
    int start = 0;
    int len = content_space(type);
    do
    {
        buffer = content.substr(start, len);
        send(type, buffer);
        start += len;
        len = content_space(type);
    } while (start < (int)content.length());
    send(Message::Type::END);
}

string Connection::receive_long_content(Message::Type type)
{
    string received_content;

    while (true)
    {
        Message msg = just_receive();
        {
            if (msg.type == type)
            {
                confirm_receipt(msg);
                received_content += msg.content;
            }
            else if (msg.type == Message::Type::END)
            {
                confirm_receipt(msg);
                return received_content;
            }
            else if (msg.type == Message::Type::ERROR)
            {
                confirm_receipt(msg);
                throw ResponseException(msg.content);
            }
        }
    }
}

void Connection::send_ack(bool ok)
{
    if (ok)
    {
        just_send(Message::Type::ACK, to_string(last_sequence_received));
    }
    else
    {
        just_send(Message::Type::ERROR, to_string(last_sequence_received));
    }
}

void Connection::send_file(string filename)
{
    ifstream file_stream(filename, ifstream::binary);
    int content_len = content_space(Message::Type::FILE);
    char *buffer = new char[content_len];
    do
    {
        file_stream.read(buffer, content_len);
        send(Message::Type::FILE, string(buffer, file_stream.gcount()));
        int new_content_len = content_space(Message::Type::FILE);
        if (new_content_len < content_len)
        {
            delete[] buffer;
            buffer = new char[new_content_len];
            content_len = new_content_len;
        }
    } while (!file_stream.eof());
    delete[] buffer;
    send(Message::Type::END);
    File file(filename);
    string modtime = to_string(file.modification_time());
    send(Message::Type::MODTIME, modtime);
}

void Connection::receive_file(string filepath)
{
    ofstream file_stream;
    file_stream.open(filepath, ofstream::binary | ofstream::trunc);
    while (true)
    {
        Message msg = receive(Message::type_file());
        if (msg.type == Message::Type::FILE)
        {
            file_stream.write(msg.content.data(), msg.content.length());
        }
        else
        {
            break;
        }
    }
    file_stream.close();
    int modification_time = stoi(receive_content(Message::Type::MODTIME));
    File file(filepath);
    file.set_modification_time(modification_time);
}

void Connection::resend()
{
    for (auto it = messages_sent.begin(); it != messages_sent.end(); ++it)
    {
        // first = sequence, second = message
        debug("Resending message " + to_string(it->first), __FILE__);
        sock->send(string(it->second));
    }
}

Message Connection::just_receive()
{
    while (true)
    {
        try
        {
            Message msg = Message::parse(sock->receive());
            msg.print('<');
            if (msg.session == session)
            {
                if (msg.sequence == last_sequence_received + 1)
                {
                    return msg;
                }
                else
                {
                    resend();
                }
            }
            else
            {
                debug("Message received from wrong session");
            }
        }
        catch (timeout_exception &e)
        {
            resend();
        }
        catch (runtime_error &e)
        {
            cout << e.what() << endl;
        }
    }
}

void Connection::receive_ack()
{
    debug("Waiting for ACK " + to_string(last_sequence_sent) + "...", __FILE__);
    while (true)
    {
        Message msg = just_receive();
        {
            if (msg.type == Message::Type::ACK && stoi(msg.content) == last_sequence_sent)
            {
                last_sequence_received = msg.sequence;
                messages_sent.clear();
                return;
            }
            else if (msg.type == Message::Type::ERROR)
            {
                last_sequence_received = msg.sequence;
                messages_sent.clear();
                throw ResponseException(msg.content);
            }
        }
    }
}

Message Connection::receive(Message::Type expected_type)
{
    debug("Waiting for " + Message::str(expected_type) + "...", __FILE__);
    while (true)
    {
        Message msg = just_receive();
        if (msg.type == expected_type)
        {
            confirm_receipt(msg);
            return msg;
        }
        else if (msg.type == Message::Type::ERROR)
        {
            confirm_receipt(msg);
            throw ResponseException(msg.content);
        }
    }
}

Message Connection::receive(list<Message::Type> expected_types)
{
    while (true)
    {
        Message msg = just_receive();
        for (Message::Type &expected : expected_types)
        {
            if (msg.type == expected || msg.type == Message::Type::ERROR)
            {
                confirm_receipt(msg);
                return msg;
            }
            else if (msg.type == Message::Type::ERROR)
            {
                confirm_receipt(msg);
                throw ResponseException(msg.content);
            }
        }
    }
}

string Connection::receive_content(Message::Type expected_type)
{
    Message msg = receive(expected_type);
    return msg.content;
}

Message Connection::receive_request()
{
    debug("Waiting request from " + session + "...", __FILE__);
    this->sock->disable_timeout();
    while (true)
    {
        Message msg = receive(Message::type_request());
        this->sock->enable_timeout();
        return msg;
    }
}

int Connection::content_space(Message::Type type)
{
    int content_space = SOCKET_BUFFER_SIZE;
    content_space -= session.length();
    content_space -= to_string(last_sequence_sent).length() + 1;
    content_space -= to_string(static_cast<int>(type)).length();
    content_space -= 4; // separators
    return content_space;
}
