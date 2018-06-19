#include "Connection.hpp"

Connection::Connection(string session, Socket *sock)
{
    this->session = session;
    if (session.empty())
    {
        this->session = to_string(rand() % 10000);
    }
    this->sock = sock;
    init_sequences();
}

Connection *Connection::listener(int port)
{
    Connection *connection = new Connection("0", new Socket(port));
    connection->sock->bind_server();
    return connection;
}

Connection::~Connection()
{
    debug("Closing connection " + session);
    delete this->sock;
}

void Connection::connect_to_host(string hostname, int port)
{
    sock = new Socket(port);
    sock->set_host(hostname);
    connect();
}

void Connection::connect()
{
    send(Message::Type::SYN); // SYN (1/3)
    sock->set_to_answer(sock);
    send_ack(); // ACK (3/3)
}

void Connection::confirm()
{
    last_sequence_received = 0;
    send_ack(); // SYN+ACK (2/3)
    receive_ack();
}

void Connection::just_send(Message::Type type, string content)
{
    last_sequence_sent += 1;
    Message msg = Message(session, last_sequence_sent, type, content);
    msg.print('>');
    sock->send(msg.stringify());
    messages_sent[last_sequence_sent] = msg.stringify();
}

bool Connection::send(Message::Type type, string content)
{
    just_send(type, content);
    return receive_ack();
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
    File file(filename);
    string modtime = to_string(file.modification_time()); // throw FileNotFound
    send(Message::Type::MODTIME, modtime);                // throw SendFail, Response

    ifstream file_stream(file.filepath, ifstream::binary);
    int content_len = content_space(Message::Type::FILE);
    char *buffer = new char[content_len];
    do
    {
        file_stream.read(buffer, content_len);
        send(Message::Type::FILE, string(buffer, file_stream.gcount()));
    } while (!file_stream.eof());
    send(Message::Type::END_OF_FILE);
}

void Connection::resend()
{
    for (auto it = messages_sent.begin(); it != messages_sent.end(); ++it)
    {
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

bool Connection::receive_ack()
{
    debug("Waiting for ACK " + to_string(last_sequence_sent) + "...");
    while (true)
    {
        Message msg = just_receive();
        {
            if (stoi(msg.content) == last_sequence_sent)
            {
                if (msg.type == Message::Type::ACK)
                {
                    last_sequence_received = msg.sequence;
                    messages_sent.clear();
                    return true;
                }
                else if (msg.type == Message::Type::ERROR)
                {
                    last_sequence_received = msg.sequence;
                    messages_sent.clear();
                    return false;
                }
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
            last_sequence_received = msg.sequence;
            return msg;
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
            if (msg.type == expected)
            {
                last_sequence_received = msg.sequence;
                return msg;
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

Connection* Connection::receive_connection()
{
    string new_session;
    sock->disable_timeout();
    while (true)
    {
        string msg_s = sock->receive();
        Message msg = Message::parse(msg_s);
        msg.print('<');
        if (msg.type == Message::Type::SYN && msg.session != session)
        {
            sock->enable_timeout();
            return new Connection(msg.session, sock->get_answerer());
        }
    }
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
                last_sequence_received = msg.sequence;
                received_content += msg.content;
                send_ack();
            }
            else if (msg.type == Message::Type::END)
            {
                last_sequence_received = msg.sequence;
                send_ack();
                return received_content;
            }
        }
    }
}

void Connection::receive_file(string filepath)
{
    int modification_time = stoi(receive_content(Message::Type::MODTIME));
    list<Message::Type> expected({Message::Type::FILE, Message::Type::END_OF_FILE});
    ofstream file_stream;
    while (true)
    {
        Message msg = receive(expected);
        if (msg.type == Message::Type::FILE)
        {
            file_stream.write(msg.content.data(), msg.content.length());
            send_ack();
        }
        else
        {
            break;
        }
    }
    send_ack();
    file_stream.close();
    File file(filepath);
    file.set_modification_time(modification_time);
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

void Connection::init_sequences()
{
    last_sequence_sent = -1;
    last_sequence_received = -1;
}
