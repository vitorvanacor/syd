#include "Connection.hpp"

Connection::Connection(string username, string session, Socket *sock)
{
    this->username = username;
    this->session = session;
    this->sock = sock;
    init_sequences();
}

Connection::~Connection()
{
    debug("Closing connection " + session);
    delete this->sock;
}

void Connection::connect(string hostname, int port)
{
    this->session = to_string(rand() % 10000);
    this->sock = new Socket(port);
    this->sock->set_host(hostname);
    send(Message::T_SYN, username);
    receive_ack();
    sock->set_dest_address(sock->get_sender_address());
    send_ack();
    user_directory = HOME+"/sync_dir_"+username;
    File::create_directory(user_directory);
}

void Connection::accept_connection()
{
    last_sequence_received = 0;
    send_ack();
    receive_ack();
    user_directory = username;
    File::create_directory(user_directory);
    debug("Connection " + session + " established");
}

void Connection::send(string type, string content)
{
    last_sequence_sent += 1;
    Message msg = Message(session, last_sequence_sent, type, content);
    msg.print('>', username);
    sock->send(msg.to_string());
    messages_sent[last_sequence_sent] = msg.to_string();
}

void Connection::send_ack()
{
    send(Message::T_ACK, to_string(last_sequence_received));
}

void Connection::send_file(string filepath)
{
    ifstream file(filepath, std::ifstream::binary);;
    char buffer[PACKET_SIZE];
    do
    {
        file.read(buffer, PACKET_SIZE);
        send(Message::T_FILE, string(buffer, file.gcount()));
        receive_ack();
    } while (!file.eof());
    send(Message::T_EOF);
    receive_ack();
}

void Connection::resend()
{
    for (map<int, string>::iterator it = messages_sent.begin(); it != messages_sent.end(); ++it)
    {
        debug("Resending message " + to_string(it->first), __FILE__);
        sock->send(string(it->second));
    }
}

void Connection::receive_ack()
{
    debug("Waiting for ACK " + to_string(last_sequence_sent) + "...");
    while (true)
    {
        Message msg = receive();
        {
            // TODO: consider that error or bye can be received too
            if (msg.type == Message::T_ACK && stoi(msg.content) == last_sequence_sent)
            {
                last_sequence_received = msg.sequence;
                messages_sent.clear();
                return;
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
        Message msg = receive();
        if (msg.type == expected_type)
        {
            last_sequence_received = msg.sequence;
            return msg;
        }
    }
}

Message Connection::receive_request()
{
    debug("Waiting request from " + username + "...", __FILE__);
    this->sock->set_timeout(0); // Never timeout
    while (true)
    {
        Message msg = receive();
        if (msg.is_request())
        {
            last_sequence_received = msg.sequence;
            this->sock->set_timeout(Socket::DEFAULT_TIMEOUT);
            return msg;
        }
    }
}

Message Connection::receive()
{
    while (true)
    {
        try
        {
            Message msg = Message::parse(sock->receive());
            msg.print('<', username);
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

void Connection::receive_file(string filename)
{
    debug("Waiting for data!");
    ofstream file(user_directory+"/"+filename, ofstream::binary | ofstream::trunc);
    while (true)
    {
        
        Message msg = receive();
        {
            // TODO: consider that error or bye can be received too
            if (msg.type == Message::T_FILE) {
                last_sequence_received = msg.sequence;
                file.write(msg.content.data(), msg.content.length());
                send_ack();
            }
            else if (msg.type == Message::T_EOF)
            {
                last_sequence_received = msg.sequence;
                send_ack();
                return;
            }
        }
    }
}

void Connection::init_sequences()
{
    last_sequence_sent = -1;
    last_sequence_received = -1;
}
