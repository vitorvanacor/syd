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

void Connection::connect(string username)
{
    send(Message::T_SYN, username);
    sock->set_dest_address(sock->get_sender_address());
    send_ack();
}

void Connection::accept_connection()
{
    last_sequence_received = 0;
    send_ack(); // SYN ACK
    receive_ack();
}

void Connection::just_send(string type, string content)
{
    last_sequence_sent += 1;
    Message msg = Message(session, last_sequence_sent, type, content);
    msg.print('>');
    sock->send(msg.to_string());
    messages_sent[last_sequence_sent] = msg.to_string();
}

void Connection::send(string type, string content)
{
    just_send(type, content);
    receive_ack();
}

void Connection::send_string(string type, string content)
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
    send(Message::T_END);
}

void Connection::send_ack(bool ok)
{
    if (ok)
    {
        just_send(Message::T_ACK, to_string(last_sequence_received));
    }
    else
    {
        just_send(Message::T_ERROR, to_string(last_sequence_received));
    }
}

int Connection::send_file(string filepath)
{
    ifstream file(filepath, std::ifstream::binary);
    const char buffer[content_space(Message::T_FILE)];
    do
    {
        file.read(buffer, content_space(Message::T_FILE));
        send(Message::T_FILE, string(buffer, file.gcount()));
    } while (!file.eof());
    send(Message::T_EOF);
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

Message Connection::receive(string expected_type)
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

string Connection::receive_string()
{
    string received_string;

    while(true)
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
