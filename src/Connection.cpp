#include "Connection.hpp"

/* Client connection */
Connection::Connection(string username, string hostname, int port)
{
    this->session = to_string(rand() % 10000);
    this->username = username;
    this->sock = new Socket(port);
    debug("Creating session " + this->session, __FILE__);
    init_sequences();

    this->sock->set_host(hostname);
    this->sock->set_timeout(TIMEOUT_IN_SECONDS);
    send(Message::T_SYN, username);
    receive_ack();
    sock->set_dest_address(sock->get_sender_address());
    send_ack();
    cout << "Successfully logged in as " << username << "!" << endl;
}

/* Server connection */
Connection::Connection(string username, string session, Socket *new_socket)
{
    debug("New Connection: " + username + " (" + session + ")");
    init_sequences();
    this->session = session;
    this->username = username;
    this->sock = new_socket;
    this->sock->set_timeout(TIMEOUT_IN_SECONDS);
}

Connection::~Connection()
{
    debug("Closing connection " + session);
    delete this->sock;
}

void Connection::accept_connection()
{
    last_sequence_received = 0;
    send_ack();
    receive_ack();
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

void Connection::send_file(File file)
{
    char buffer[PACKET_SIZE];
    FILE *file_ptr;
    int total_send = 0;

    string file_path = file.GetPath();
    file_ptr = fopen(file_path.c_str(), "rb+");

    // File length
    ifstream in(file_path, ifstream::ate | ifstream::binary);
    int file_length = in.tellg();

    int remaining_bytes = file_length;
    
    int file_index = 0;
    while (remaining_bytes >= PACKET_SIZE)
    {
        total_send += sizeof(buffer);

        fread(buffer, sizeof(char), PACKET_SIZE, file_ptr);
        send(Message::T_FILE, buffer);
        receive_ack();
        file_index += PACKET_SIZE;
        remaining_bytes -= PACKET_SIZE;
    }
    if (remaining_bytes > 0)
    {
        char remainder_buffer[remaining_bytes];
        fread(remainder_buffer, sizeof(char), remaining_bytes, file_ptr);

        total_send += sizeof(remainder_buffer);

        send(Message::T_FILE, remainder_buffer);
        receive_ack();
    }

    cout << "total_send: " << total_send << endl;

    fclose(file_ptr);

    // Send end of file
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
    sock->set_timeout(TIMEOUT_IN_SECONDS);
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
    sock->set_timeout(TIMEOUT_IN_SECONDS);
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

void Connection::receive_file()
{
    debug("Waiting for data!");
    sock->set_timeout(TIMEOUT_IN_SECONDS);
    FILE *file_ptr = fopen("/home/pietra/Documentos/UFRGS/CIC/SISOP2/syd/banana.jpg", "wb+");
    //char buffer[PACKET_SIZE];
    int length = 0;

    while (true)
    {
        debug("Waiting for data!");
        Message msg = receive();
        {
            // TODO: consider that error or bye can be received too
            if (msg.type == Message::T_FILE)
            {
                last_sequence_received = msg.sequence;
                //cout << "Content: " << msg.content << endl;

                char *buffer = new char[msg.content.length() + 1]; //LEAK!!

                //strcpy(buffer, msg.content.data()); vitor tirou. memcpy seria melhor
                length += msg.content.length();

                for (unsigned int i = 0; i < sizeof(buffer); i++)
                    printf(" %d ", buffer[i]);

                //fwrite(buffer, sizeof(char), sizeof(buffer), file_ptr); vitor tirou
                fwrite(msg.content.data(), sizeof(char), sizeof(msg.content.data()), file_ptr);
                send_ack();
            }
            else if (msg.type == Message::T_EOF)
            {
                last_sequence_received = msg.sequence;
                cout << "End of File!" << endl;
                cout << "received: " << length << endl;
                fclose(file_ptr);
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
