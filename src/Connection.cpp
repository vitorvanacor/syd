#include "Connection.hpp"

#include <utime.h>

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
    send(Message::Type::SYN);
    sock->set_to_answer();
    send_ack();
}

void Connection::accept_connection()
{
    last_sequence_received = 0;
    send_ack(); // SYN ACK
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
    } while (start < content.length());
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

int Connection::send_file(File file)
{
    send(Message::Type::MODTIME, to_string(file.modification_time())); // throw SendFail, Response

    ifstream file_stream(file.filepath, std::ifstream::binary);
    const char buffer[content_space(Message::Type::FILE)];
    do
    {
        file_stream.read(buffer, content_space(Message::Type::FILE));
        send(Message::Type::FILE, string(buffer, file_stream.gcount()));
    } while (!file_stream.eof());
    send(Message::Type::END_OF_FILE);
    return 0;
}

void Connection::resend()
{
    for (auto it = messages_sent.begin(); it != messages_sent.end(); ++it)
    {
        debug("Resending message " + to_string(it->first), __FILE__);
        sock->just_send(string(it->second));
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
                if (msg.type == Message::T_ACK)
                {
                    last_sequence_received = msg.sequence;
                    messages_sent.clear();
                    return true;
                }
                else if (msg.type == Message::T_ERROR)
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
    debug("Waiting for " + expected_type + "...", __FILE__);
    while (true)
    {
        // TODO: consider that error or bye can be received too
        Message msg = just_receive();
        if (msg.type == expected_type)
        {
            last_sequence_received = msg.sequence;
            return msg;
        }
    }
}

string Connection::receive_content(Message::Type expected_type)
{
    Message msg = receive(expected_type);
    return msg.content;
}

Message Connection::receive(list<string> expected_types)
{
    while (true)
    {
        // TODO: consider that error or bye can be received too
        Message msg = just_receive();
        for (auto it = expected_types.begin(); it != expected_types.end(); ++it)
        {
            if (*it == msg.type)
            {
                last_sequence_received = msg.sequence;
                return msg;
            }
        }
    }
}

Message Connection::receive_request()
{
    debug("Waiting request from " + session + "...", __FILE__);
    this->sock->set_timeout(0); // Never timeout
    while (true)
    {
        Message msg = just_receive();
        if (msg.is_request())
        {
            last_sequence_received = msg.sequence;
            this->sock->set_timeout(Socket::DEFAULT_TIMEOUT);
            return msg;
        }
    }
}

Connection *Connection::receive_connection()
{
    string new_session;
    sock->set_timeout(0); // Never timeout
    while (true)
    {
        string msg_s = sock->receive();
        Message msg = Message::parse(msg_s);
        if (msg.type == Message::Type::SYN && msg.session != session)
        {
            sock->set_timeout(Socket::DEFAULT_TIMEOUT);
            return new Connection(msg.session, sock->get_answerer());
        }
    }
}

string Connection::receive_long_message()
{
    string received_string;

    while (true)
    {
        Message msg = just_receive();
        {
            if (msg.type == Message::T_END)
            {
                last_sequence_received = msg.sequence;
                send_ack();
                return received_string;
            }
            else
            {
                last_sequence_received = msg.sequence;
                received_string += msg.content;
                send_ack();
            }
        }
    }
}

void Connection::receive_file(string filepath)
{
    debug("Waiting for file!");

    ofstream file;
    int modification_time;

    while (true)
    {
        Message msg = just_receive();
        {
            if (msg.type == Message::T_MODTIME)
            {
                last_sequence_received = msg.sequence;
                modification_time = stoi(msg.content);

                file.open(filepath, ofstream::binary | ofstream::trunc);

                send_ack();
            }
            else if (msg.type == Message::T_FILE)
            {
                last_sequence_received = msg.sequence;
                file.write(msg.content.data(), msg.content.length());
                send_ack();
            }
            else if (msg.type == Message::T_EOF)
            {
                last_sequence_received = msg.sequence;
                send_ack();
                file.close();

                // Sets modification time
                struct utimbuf ubuf;
                ubuf.modtime = modification_time;
                if (utime(filepath.c_str(), &ubuf) != 0)
                {
                    perror("utime() error");
                }
                return 0;
            }
            else if (msg.type == Message::T_ERROR)
            {

                last_sequence_received = msg.sequence;
                send_ack();
                return -1;
            }
        }
    }
}

void Connection::init_sequences()
{
    last_sequence_sent = -1;
    last_sequence_received = -1;
}
